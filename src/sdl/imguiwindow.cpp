#include "imguiwindow.h"

#include <spdlog/spdlog.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

namespace sdl {

	namespace {

		void showColorWindow(bool& open) {
			ImGui::SetNextWindowSize({310.f, 400.f});
			ImGui::Window("Html Colors", &open, ImGuiWindowFlags_NoResize, []() {
				ImGui::Text("Copy color to clipboard by clicking");

				static const auto htmlColors = color::html::getHtmlColors();
				static std::string colorStr = "                   ";
				int nbr = 0;
				for (const auto& [name, color] : htmlColors) {
					++nbr;
					colorStr.clear();
					colorStr += name;
					colorStr += "##1";
					if (ImGui::ColorButton(colorStr.c_str(), color)) {
						ImGui::SetClipboardText(name);
					}
					if (nbr % 10 != 0) {
						ImGui::SameLine();
					}
				}
			});
		}

	}

	ImGuiWindow::~ImGuiWindow() {
		SDL_WaitForGPUIdle(getSdlGpuDevice());
		ImGui_ImplSDLGPU3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiWindow::initPreLoop() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplSDL3_InitForSDLGPU(getSdlWindow());
		ImGui_ImplSDLGPU3_InitInfo init_info = {};
		init_info.Device = getSdlGpuDevice();
		init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(init_info.Device, getSdlWindow());
		init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
		ImGui_ImplSDLGPU3_Init(&init_info);
	}

	void ImGuiWindow::update(const DeltaTime& deltaTime) {
		imGuiPreUpdate(deltaTime);
		ImGui_ImplSDLGPU3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		imGuiUpdate(deltaTime);

		if (showDemoWindow_) {
			ImGui::ShowDemoWindow(&showDemoWindow_);
		}
		if (showColorWindow_) {
			showColorWindow(showColorWindow_);
		}

		ImGui::Render();

		ImDrawData* draw_data = ImGui::GetDrawData();
		const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

		SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(getSdlGpuDevice()); // Acquire a GPU command buffer

		SDL_GPUTexture* swapchain_texture;
		SDL_AcquireGPUSwapchainTexture(command_buffer, getSdlWindow(), &swapchain_texture, nullptr, nullptr); // Acquire a swapchain texture

		if (swapchain_texture != nullptr && !is_minimized) {
			// This is mandatory: call Imgui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
			ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

			// Setup and start a render pass
			SDL_GPUColorTargetInfo target_info = {};
			target_info.texture = swapchain_texture;
			auto clearColor = getClearColor();
			target_info.clear_color = SDL_FColor{clearColor.red(), clearColor.green(), clearColor.blue(), clearColor.alpha()};
			target_info.load_op = SDL_GPU_LOADOP_CLEAR;
			target_info.store_op = SDL_GPU_STOREOP_STORE;
			target_info.mip_level = 0;
			target_info.layer_or_depth_plane = 0;
			target_info.cycle = false;
			SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

			// Render ImGui
			ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

			SDL_EndGPURenderPass(render_pass);
		}
		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		SDL_SubmitGPUCommandBuffer(command_buffer);

		imGuiPostUpdate(deltaTime);
	}

	void ImGuiWindow::eventUpdate(const SDL_Event& windowEvent) {
		ImGui_ImplSDL3_ProcessEvent(&windowEvent);
		
		auto& io = ImGui::GetIO();
		bool ioWantCapture = false;
		switch (windowEvent.type) {
			case SDL_EVENT_MOUSE_BUTTON_UP:
				[[fallthrough]];
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				[[fallthrough]];
			case SDL_EVENT_MOUSE_MOTION:
				[[fallthrough]];
			case SDL_EVENT_MOUSE_WHEEL:
				ioWantCapture = io.WantCaptureMouse;
				break;
			case SDL_EVENT_KEY_UP:
				[[fallthrough]];
			case SDL_EVENT_KEY_DOWN:
				ioWantCapture = io.WantCaptureKeyboard;
				break;
			case SDL_EVENT_TEXT_EDITING:
				[[fallthrough]];
			case SDL_EVENT_TEXT_INPUT:
				ioWantCapture = io.WantTextInput;
				break;
		}

		if (!ioWantCapture) {
			imGuiEventUpdate(windowEvent);
		}
	}

}
