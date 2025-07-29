#include "window.h"
#include "sdlexception.h"

#include <spdlog/spdlog.h>
#include <SDL3_image/SDL_image.h>

#include <thread>
#include <chrono>
#include <sstream>
#include <stdexcept>

#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

namespace sdl {

	namespace {

		void imGuiInit(SDL_Window* window, SDL_GPUDevice* device) {
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
			ImGui_ImplSDL3_InitForSDLGPU(window);
			ImGui_ImplSDLGPU3_InitInfo init_info = {};
			init_info.Device = device;
			init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(init_info.Device, window);
			init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
			ImGui_ImplSDLGPU3_Init(&init_info);
		}

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

		[[nodiscard]] SDL_GPUDevice* initialize(SDL_Window* window) {
			int driversNbr = SDL_GetNumGPUDrivers();
			for (int i = 0; i < driversNbr; ++i) {
				spdlog::info("[GpuContext] Preferred GPU driver: {}", SDL_GetGPUDriver(i));
			}

			auto gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL, true, nullptr);
			if (!gpuDevice) {
				throw SdlException{"[GpuContext] Failed to create GPU device: {}", SDL_GetError()};
			}

			if (!SDL_ClaimWindowForGPUDevice(gpuDevice, window)) {
				SDL_DestroyGPUDevice(gpuDevice);
				throw SdlException{"[GpuContext] Failed to claim window for GPU device: {}", SDL_GetError()};
			}
			return gpuDevice;
		}

	}

	Window::Window() {
		spdlog::info("[sdl::Window] Creating Window");

		// SDL_VERSION hardcoded number from SDL headers
		spdlog::info("[sdl::Window] Compiled SDL Version: {}.{}.{}", 
			SDL_VERSIONNUM_MAJOR(SDL_VERSION),
			SDL_VERSIONNUM_MINOR(SDL_VERSION),
			SDL_VERSIONNUM_MICRO(SDL_VERSION)
		);
		const int linked = SDL_GetVersion(); // reported by linked SDL library
		spdlog::info("[sdl::Window] Linked SDL Version: {}.{}.{}",
			SDL_VERSIONNUM_MAJOR(linked),
			SDL_VERSIONNUM_MINOR(linked),
			SDL_VERSIONNUM_MICRO(linked));
	}

	Window::~Window() {
		if (icon_) {
			SDL_DestroySurface(icon_);
		}
		
		ImGui_ImplSDLGPU3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();

		if (gpuDevice_) {
			SDL_WaitForGPUIdle(gpuDevice_);

			if (window_) {
				SDL_ReleaseWindowFromGPUDevice(gpuDevice_, window_);
			}
			SDL_DestroyGPUDevice(gpuDevice_);
		}

		if (window_) {
			SDL_DestroyWindow(window_);
			SDL_Quit();
		}

		spdlog::debug("[sdl::Window] Destructed.");
	}

	void Window::startLoop() {
		if (window_) {
			spdlog::warn("[sdl::Window] Loop already running");
			return;
		}

		spdlog::info("[sdl::Window] Init loop");
		window_ = SDL_CreateWindow(
			title_.c_str(),
			width_,	height_,
			flags_
		);
		if (window_ == nullptr) {
			throw sdl::SdlException{"[sdl::Window] Failed to create window"};
		} else {
			spdlog::info("[sdl::Window] Windows {} created: \n\t(x, y) = ({}, {}) \n\t(w, h) = ({}, {}) \n\tflags = {}",
				title_, 
				x_ == SDL_WINDOWPOS_UNDEFINED? -1 : x_,
				y_ == SDL_WINDOWPOS_UNDEFINED ? -1 : y_,
				width_,
				height_,
				flags_
			);
		}
		gpuDevice_ = initialize(window_);

		if (!SDL_SetGPUSwapchainParameters(gpuDevice_, window_, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC)) {
			spdlog::warn("[sdl::Window] SDL_SetGPUSwapchainParameters failed: {}", SDL_GetError());
		}

		if (icon_) {
			spdlog::debug("[sdl::Window] Windows icon updated");
			SDL_SetWindowIcon(window_, icon_);
			SDL_DestroySurface(icon_);
			icon_ = nullptr;
		}
		quit_ = false;
		setHitTestCallback(onHitTest_);

		imGuiInit(window_, gpuDevice_);
		preLoop();
		spdlog::info("[sdl::Window] Loop starting");
		runLoop();
		spdlog::info("[sdl::Window] Loop ended");
		postLoop();
	}

	void Window::runLoop() {
		auto time = Clock::now();
		while (!quit_) {
			SDL_Event eventSDL;
			while (SDL_PollEvent(&eventSDL)) {
				ImGui_ImplSDL3_ProcessEvent(&eventSDL);

				auto& io = ImGui::GetIO();
				bool ioWantCapture = false;
				switch (eventSDL.type) {
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
					processEvent(eventSDL);
				}
			}

			auto currentTime = Clock::now();
			auto delta = currentTime - time;
			time = currentTime;
			renderFrame(delta);

			if (sleepingTime_ > std::chrono::nanoseconds{0}) {
				std::this_thread::sleep_for(sleepingTime_);
			}
		}
	}

	void Window::renderFrame(const DeltaTime& deltaTime) {
		ImGui_ImplSDLGPU3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		renderImGui(deltaTime);

		if (showDemoWindow_) {
			ImGui::ShowDemoWindow(&showDemoWindow_);
		}
		if (showColorWindow_) {
			showColorWindow(showColorWindow_);
		}

		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();
		const bool isMinimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);

		SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(gpuDevice_);

		SDL_GPUTexture* swapchainTexture;
		// Uses of SDL_AcquireGPUSwapchainTexture makes memory ballon! So uses SDL_WaitAndAcquireGPUSwapchainTexture instead.
		SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window_, &swapchainTexture, nullptr, nullptr);
		
		if (swapchainTexture != nullptr && !isMinimized) {
			// Derived class can override this method to draw additional content.
			renderFrame(deltaTime, swapchainTexture, commandBuffer);

			ImGui_ImplSDLGPU3_PrepareDrawData(drawData, commandBuffer);

			SDL_GPUColorTargetInfo targetInfo{
				.texture = swapchainTexture,
				.load_op = SDL_GPU_LOADOP_LOAD,
			};
			SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &targetInfo, 1, nullptr);

			ImGui_ImplSDLGPU3_RenderDrawData(drawData, commandBuffer, renderPass);
			SDL_EndGPURenderPass(renderPass);
		}
		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		SDL_SubmitGPUCommandBuffer(commandBuffer);
	}

	void Window::renderFrame(const DeltaTime& deltaTime, SDL_GPUTexture* swapchainTexture, SDL_GPUCommandBuffer* commandBuffer) {
		SDL_GPUColorTargetInfo targetInfo{
			.texture = swapchainTexture,
			.clear_color = clearColor_,
			.load_op = SDL_GPU_LOADOP_CLEAR,
			.store_op = SDL_GPU_STOREOP_STORE,
		};
		SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &targetInfo, 1, nullptr);
		SDL_EndGPURenderPass(renderPass);
	}

	void Window::setPosition(int x, int y) {
		if (window_) {
			if (x < 0) {
				x = SDL_WINDOWPOS_UNDEFINED;
			}
			if (y < 0) {
				y = SDL_WINDOWPOS_UNDEFINED;
			}			
			SDL_SetWindowPosition(window_, x, y);
			spdlog::info("[sdl::Window] Reposition window: (x, y) = ({}, {})", x == SDL_WINDOWPOS_UNDEFINED ? -1 : x, y == SDL_WINDOWPOS_UNDEFINED ? -1 : y);
		} else {
			x_ = x;
			y_ = y;
		}
	}
	void Window::setIcon(const std::string& iconPath) {
		if (icon_) {
			SDL_DestroySurface(icon_);
			icon_ = nullptr;
		}
		icon_ = IMG_Load(iconPath.c_str());
		if (!icon_) {
			spdlog::error("[sdl::Window] Failed to load icon: {}", SDL_GetError());
			return;
		}
		if (window_) {
			SDL_SetWindowIcon(window_, icon_);
			SDL_DestroySurface(icon_);
			icon_ = nullptr;
		}
	}

	void Window::setTitle(const std::string& title) {
		if (window_) {
			spdlog::info("[sdl::Window] tile named to {}", title);
			SDL_SetWindowTitle(window_, title.c_str());
		} else {
			title_ = title;
		}
	}	
	
	void Window::setSize(int width, int height) {
		if (window_) {
			if (width < 1 || height < 1) {
				spdlog::warn("[sdl::Window] Resizing: (w, h) = ({}, {})", width, height);
				return;
			}

			spdlog::info("[sdl::Window] Resizing: (w, h) = ({}, {})", width, height);
			SDL_SetWindowSize(window_, width, height);
		} else {
			width_ = width;
			height_ = height;
		}
	}
	SDL_HitTestResult Window::hitTestCallback(SDL_Window* sdlWindow, const SDL_Point* area, void* data) {
		return static_cast<Window*>(data)->onHitTest_(*area);
	}

	void Window::setHitTestCallback(HitTestCallback onHitTest) {
		onHitTest_ = std::move(onHitTest);
		if (onHitTest_) {
			if (!SDL_SetWindowHitTest(window_, hitTestCallback, this)) {
				spdlog::warn("[sdl::Window] Window hit test failed to be initiated: {}", SDL_GetError());
			}
		} else {
			SDL_SetWindowHitTest(window_, nullptr, this);
		}
	}

}
