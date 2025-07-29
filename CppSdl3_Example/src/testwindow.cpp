#include "testwindow.h"

#include <spdlog/spdlog.h>
#include <fmt/core.h>

#include <SDL3_image/SDL_image.h>

namespace {

	void addSquare(std::vector<sdl::Vertex>& vertices, glm::vec3 position, float size, sdl::Color color) {
		// Bottom-left triangle
		vertices.push_back({position + glm::vec3(-size, -size, 0.0f), {}, color});
		vertices.push_back({position + glm::vec3(size, -size, 0.0f), {}, color});
		vertices.push_back({position + glm::vec3(-size, size, 0.0f), {}, color});
		// Top-right triangle
		vertices.push_back({position + glm::vec3(size, -size, 0.0f), {}, color});
		vertices.push_back({position + glm::vec3(size, size, 0.0f), {}, color});
		vertices.push_back({position + glm::vec3(-size, size, 0.0f), {}, color});
	}

	void addSquareTexture(std::vector<sdl::Vertex>& vertices, glm::vec3 position, float size, sdl::Color color) {
		// Bottom-left triangle
		vertices.push_back({position + glm::vec3(-size, -size, 0.0f), {0.f, 0.f}, color});
		vertices.push_back({position + glm::vec3(size, -size, 0.0f), {1.f, 0.f}, color});
		vertices.push_back({position + glm::vec3(-size, size, 0.0f), {0.f, 1.f}, color});
		// Top-right triangle
		vertices.push_back({position + glm::vec3(size, -size, 0.0f), {1.f, 0.f}, color});
		vertices.push_back({position + glm::vec3(size, size, 0.0f), {1.f, 1.f}, color});
		vertices.push_back({position + glm::vec3(-size, size, 0.0f), {0.f, 1.f}, color});
	}

	void printGameControllerButton(Uint8 button) {
		switch (button) {
			case SDL_GAMEPAD_BUTTON_SOUTH:
				fmt::println("SDL_GAMEPAD_BUTTON_SOUTH");
				break;
			case SDL_GAMEPAD_BUTTON_EAST:
				fmt::println("SDL_GAMEPAD_BUTTON_EAST");
				break;
			case SDL_GAMEPAD_BUTTON_WEST:
				fmt::println("SDL_GAMEPAD_BUTTON_WEST");
				break;
			case SDL_GAMEPAD_BUTTON_NORTH:
				fmt::println("SDL_GAMEPAD_BUTTON_NORTH");
				break;
			case SDL_GAMEPAD_BUTTON_BACK:
				fmt::println("SDL_GAMEPAD_BUTTON_BACK");
				break;
			case SDL_GAMEPAD_BUTTON_GUIDE:
				fmt::println("SDL_GAMEPAD_BUTTON_GUIDE");
				break;
			case SDL_GAMEPAD_BUTTON_START:
				fmt::println("SDL_GAMEPAD_BUTTON_START");
				break;
			case SDL_GAMEPAD_BUTTON_LEFT_STICK:
				fmt::println("SDL_GAMEPAD_BUTTON_LEFT_STICK");
				break;
			case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
				fmt::println("SDL_GAMEPAD_BUTTON_RIGHT_STICK");
				break;
			case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
				fmt::println("SDL_GAMEPAD_BUTTON_LEFT_SHOULDER");
				break;
			case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
				fmt::println("SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER");
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_UP:
				fmt::println("SDL_GAMEPAD_BUTTON_DPAD_UP");
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
				fmt::println("SDL_GAMEPAD_BUTTON_DPAD_DOWN");
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
				fmt::println("SDL_GAMEPAD_BUTTON_DPAD_LEFT");
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
				fmt::println("SDL_GAMEPAD_BUTTON_DPAD_RIGHT");
				break;
			case SDL_GAMEPAD_BUTTON_COUNT:
				fmt::println("SDL_GAMEPAD_BUTTON_COUNT");
				break;
		}
	}

}

SDL_Surface* createSurface(int w, int h, sdl::Color color) {
	auto s = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
	SDL_FillSurfaceRect(s, nullptr, color.toImU32());
	return s;
}

TestWindow::TestWindow() {
	sdl::Window::setSize(512, 512);
	sdl::Window::setTitle("Test");
	sdl::Window::setIcon("tetris.bmp");
	sdl::Window::setShowDemoWindow(true);
	sdl::Window::setShowColorWindow(true);
}

void TestWindow::processEvent(const SDL_Event& windowEvent) {
	switch (windowEvent.type) {
		case SDL_EVENT_QUIT:
			quit();
			break;
		case SDL_EVENT_MOUSE_WHEEL: {
			auto add = windowEvent.wheel.y * 5;
			int w, h;
			SDL_GetWindowSize(getSdlWindow(), &w, &h);
			fmt::println("{} add = {}", windowEvent.wheel.which, add);

			SDL_SetWindowSize(getSdlWindow(), static_cast<int>(add +  w), static_cast<int>(add + h));
			break;
		}
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			if (windowEvent.window.windowID == getId()) {
				quit();
			}
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			fmt::println("SDL_EVENT_WINDOW_RESIZED");
			break;
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			fmt::println("SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED");
			break;
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			break;
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			break;
		case SDL_EVENT_KEY_DOWN:
			switch (windowEvent.key.key) {
				case SDLK_C:
					for (auto& gamepad : gameControllers_) {
						fmt::println("{}", gamepad.getName());
					}
					break;
				case SDLK_SPACE:
					if (onSpacePressed_) {
						onSpacePressed_();
					}
					break;
				case SDLK_ESCAPE:
					quit();
					break;
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (windowEvent.button.button == SDL_BUTTON_LEFT) {
				if (windowEvent.button.clicks == 2) {
					SDL_SetWindowFullscreen(getSdlWindow(), SDL_GetWindowFlags(getSdlWindow()) & SDL_WINDOW_FULLSCREEN);
				}
			}
			fmt::println("DOWN");
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			fmt::println("UP");
			break;
		case SDL_EVENT_GAMEPAD_ADDED:
			fmt::println("\nSDL_CONTROLLERDEVICEADDED");
			fmt::println("ControllerEvent: {}", ++controllerEvent_);
			fmt::println("Timestamp: {}", windowEvent.cdevice.timestamp);
			fmt::println("Type: {}", (int) windowEvent.cdevice.type);
			fmt::println("Which (Joystick index): {}", windowEvent.cdevice.which);
			gameControllers_.push_back(sdl::GameController::addController(windowEvent.cdevice.which));
			break;
		case SDL_EVENT_GAMEPAD_REMAPPED:
			fmt::println("SDL_CONTROLLERDEVICEREMOVED");
			fmt::println("ControllerEvent: {}", ++controllerEvent_);
			fmt::println("Timestamp: {}", windowEvent.cdevice.timestamp);
			fmt::println("Type: {}", (int) windowEvent.cdevice.type);
			fmt::println("Which (InstanceId): {}", windowEvent.cdevice.which);
			break;
		case SDL_EVENT_GAMEPAD_REMOVED:
			fmt::println("\nSDL_CONTROLLERDEVICEREMOVED");
			fmt::println("ControllerEvent: {}", ++controllerEvent_);
			fmt::println("Timestamp: {}", windowEvent.cdevice.timestamp);
			fmt::println("Type: {}", (int) windowEvent.cdevice.type);
			fmt::println("Which (InstanceId): {}", windowEvent.cdevice.which);
			removeGamepad(windowEvent.cdevice.which);
			break;
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			printGameControllerButton(windowEvent.gbutton.button);
			fmt::println("SDL_CONTROLLERBUTTONDOWN");
			fmt::println("ControllerEvent: {}", ++controllerEvent_);
			fmt::println("Timestamp: {}", windowEvent.gbutton.timestamp);
			fmt::println("Type: {}", (int) windowEvent.gbutton.type);
			fmt::println("Which: {}", windowEvent.gbutton.which);
			break;
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			break;
		case SDL_EVENT_GAMEPAD_AXIS_MOTION:
			switch (windowEvent.gaxis.axis) {
				case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
					if (windowEvent.gaxis.value > 15000) {
						fmt::println("SDL_CONTROLLER_AXIS_TRIGGERLEFT");
					}
					break;
				case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
					if (windowEvent.gaxis.value > 15000) {
						fmt::println("SDL_CONTROLLER_AXIS_TRIGGERRIGHT");
					}
					break;
			}

			break;
	}
}

void TestWindow::renderFrame(const sdl::DeltaTime& deltaTime, SDL_GPUTexture* swapchainTexture, SDL_GPUCommandBuffer* commandBuffer) {
	SDL_GPUColorTargetInfo targetInfo{
		.texture = swapchainTexture,
		.clear_color = clearColor_,
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_STORE,
	};

	SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &targetInfo, 1, nullptr);

	SDL_GPUBufferBinding binding = {
		.buffer = myVertexBuffer_->get(),
		.offset = 0
	};

	glm::mat4 modelMatrix{1};
	shader_.pushProjectionMatrix(commandBuffer, modelMatrix);
		
	SDL_BindGPUVertexBuffers(
		renderPass, 
		0,
		&binding,
		1
	);
	SDL_BindGPUGraphicsPipeline(renderPass, myGraphicsPipeline_->get());
	
	// Draw the first trianges with the first texture
	SDL_GPUTextureSamplerBinding samplerBinding{
		.texture = texture_.get(),
		.sampler = sampler_.get()
	};
	SDL_BindGPUFragmentSamplers(
		renderPass,
		0,
		&samplerBinding,
		1
	);
	SDL_DrawGPUPrimitives(
		renderPass,
		(Uint32) vertexes_.size() - 6,
		1,
		0,
		0
	);

	// Draw the last triangles with the second texture
	samplerBinding = SDL_GPUTextureSamplerBinding{
		.texture = atlas_.get(),
		.sampler = sampler_.get()
	};
	SDL_BindGPUFragmentSamplers(
		renderPass,
		0,
		&samplerBinding,
		1
	);
	SDL_DrawGPUPrimitives(
		renderPass,
		6,
		1,
		(Uint32) vertexes_.size() - 6,
		0 
	);

	SDL_EndGPURenderPass(renderPass);
}

void TestWindow::addSurfaceToAtlas(SDL_Surface* surface, int border) {
	fmt::println("Adding surface to atlas: {}x{}, format: {}", 
		surface->w, surface->h, SDL_GetPixelFormatName(surface->format));
	sdl::gpu::blitToTexture(gpuDevice_, atlas_, imageAtlas_, surface, border);
}

void TestWindow::preLoop() {
	sampler_ = sdl::gpu::createSampler(gpuDevice_, SDL_GPUSamplerCreateInfo{
		.min_filter = SDL_GPU_FILTER_NEAREST,
		.mag_filter = SDL_GPU_FILTER_NEAREST,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE
		});

	auto surface = sdl::makeSdlUnique<SDL_Surface, SDL_DestroySurface>(IMG_Load("tetris.bmp"));
	texture_ = sdl::gpu::uploadSurface(gpuDevice_, surface.get());

	auto transparentSurface = sdl::createSdlSurface(createSurface(600, 600, sdl::color::Transparent));
	atlas_ = sdl::gpu::uploadSurface(gpuDevice_, transparentSurface.get());

	sdl::GameController::loadGameControllerMappings("gamecontrollerdb.txt");
	//setHitTestCallback([](const SDL_Point&) { return SDL_HITTEST_DRAGGABLE; });

	// --- Setup Rectangle Vertex Data ---
	addSquare(vertexes_, glm::vec3(-0.5f, -0.5f, 0.0f), 0.2f, sdl::color::Red);
	addSquareTexture(vertexes_, glm::vec3(0.7f, 0.7f, 0.0f), 0.2f, sdl::color::White);

	// Triangle 1 (bottom-left)
	vertexes_.push_back({{-0.5f, -0.5f, 0.0f}, {}, sdl::color::Red});
	vertexes_.push_back({{ 0.5f, -0.5f, 0.0f}, {}, sdl::color::Green});
	vertexes_.push_back({{-0.5f,  0.5f, 0.0f}, {}, sdl::color::Blue});

	// Triangle 2 (top-right);
	vertexes_.push_back({{ 0.5f, -0.5f, 0.0f}, {}, sdl::color::Green});
	vertexes_.push_back({{ 0.5f,  0.5f, 0.0f}, {}, sdl::color::html::Yellow});
	vertexes_.push_back({{-0.5f,  0.5f, 0.0f}, {}, sdl::color::Blue});

	// To be used with the atlas texture
	addSquareTexture(vertexes_, glm::vec3(0.0, 0.0f, 0.0f), 1.0f, sdl::color::White);

	auto device = gpuDevice_;

	SDL_GPUBufferCreateInfo bufferInfo{
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = (Uint32) vertexes_.size() * sizeof(sdl::Vertex)
	};

	myVertexBuffer_ = std::make_unique<sdl::gpu::GpuBuffer>(sdl::gpu::createBuffer(gpuDevice_, bufferInfo));

	// create the vertex buffer
	SDL_GPUTransferBufferCreateInfo transferInfo{
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = (Uint32) vertexes_.size() * sizeof(sdl::Vertex)
	};
	sdl::gpu::GpuTransferBuffer transferBuffer = sdl::gpu::createTransferBuffer(gpuDevice_, transferInfo);

	// map the transfer buffer to a pointer
	mapTransferBuffer(gpuDevice_, transferBuffer, vertexes_);

	// start a copy pass
	SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
	sdl::gpu::copyPass(commandBuffer, [&](SDL_GPUCopyPass* copyPass) {
		// where is the data
		SDL_GPUTransferBufferLocation location{
			.transfer_buffer = transferBuffer.get(),
			.offset = 0
		};

		// where to upload the data
		SDL_GPUBufferRegion region{
			.buffer = myVertexBuffer_->get(),
			.size = (Uint32) vertexes_.size() * sizeof(sdl::Vertex)
		};

		// upload the data
		SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
	});
	SDL_SubmitGPUCommandBuffer(commandBuffer);

	// Create the graphics pipeline -------------------------

	// describe the vertex buffers
	SDL_GPUVertexBufferDescription vertexBufferDescriptions{
		.slot = 0,
		.pitch = sizeof(sdl::Vertex),
		.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX
	};

	SDL_GPUColorTargetDescription colorTargetDescription{
		.format = SDL_GetGPUSwapchainTextureFormat(gpuDevice_, getSdlWindow()),
		.blend_state = SDL_GPUColorTargetBlendState{
			.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
			.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
			.color_blend_op = SDL_GPU_BLENDOP_ADD,
			.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
			.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
			.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
			.enable_blend = true,
	}
	};

	shader_.load(gpuDevice_);

	// Bind the vertex and fragment shaders to the pipeline
	SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{
		.vertex_shader = shader_.vertexShader.get(),
		.fragment_shader = shader_.fragmentShader.get(),
		.vertex_input_state = SDL_GPUVertexInputState{
			.vertex_buffer_descriptions = &vertexBufferDescriptions,
			.num_vertex_buffers = 1,
			.vertex_attributes = shader_.attributes.data(),
			.num_vertex_attributes = (Uint32) shader_.attributes.size()
	},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.target_info = SDL_GPUGraphicsPipelineTargetInfo{
			.color_target_descriptions = &colorTargetDescription,
			.num_color_targets = 1,
	}
	};
	myGraphicsPipeline_ = std::make_unique<sdl::gpu::GpuGraphicsPipeline>(sdl::gpu::createGraphicsPipeline(gpuDevice_, pipelineInfo));
	if (!myGraphicsPipeline_) {
		spdlog::error("Failed to create graphics pipeline: {}", SDL_GetError());
		return;
	}
}

void TestWindow::renderImGui(const sdl::DeltaTime& deltaTime) {
	ImGui::MainWindow("Main", [&]() {
		ImGui::Button("Hello", {100, 100});
		if (ImGui::Button("Hello2", {50, 50})) {
			spdlog::info("Hello2 pressed");
		}
		//ImGui::Image(sprite_, glm::vec2{124, 124});
		//ImGui::Image(sprite_, glm::vec2{64, 64});
		//ImGui::ImageWithBg(sprite_, glm::vec2{64, 64}, sdl::color::Red, sdl::color::Green);
		//ImGui::Hexagon(sprite_, 64.f, true);
		//static int counter = 0;
		//if (ImGui::ImageButton("Button", sprite_, glm::vec2{32, 32})) { spdlog::info("Button pressed {}", ++counter); }
	});
}

void TestWindow::removeGamepad(SDL_JoystickID instanceId) {
	auto it = std::find_if(gameControllers_.begin(), gameControllers_.end(), [instanceId](const sdl::GameController& gameController) {
		return gameController.getInstanceId() == instanceId;
	});

	if (it != gameControllers_.end()) {
		gameControllers_.erase(it);
		spdlog::info("Gamepad removed: {}", instanceId);
	} else {
		spdlog::warn("Gamepad failed to be removed: {}", instanceId);
	}
}
