#ifndef TESTIMGUIWINDOW_H
#define TESTIMGUIWINDOW_H

#include <sdl/shader.h>
#include <sdl/window.h>
#include <sdl/gamecontroller.h>
#include <sdl/sdlgpu.h>
#include <sdl/imageatlas.h>

#include <functional>
#include <vector>

#include <spdlog/spdlog.h>

sdl::SdlSurface createSdlSurface(int w, int h, sdl::Color color);

sdl::SdlSurface createSdlSurface(const std::string& fileName);

class TestWindow : public sdl::Window {
public:
	TestWindow();

	void setSpaceFunction(std::function<void()> onSpacePressed) {
		onSpacePressed_ = std::move(onSpacePressed);
	}

	void addSurfaceToAtlas(SDL_Surface* surface, int border = 0);

private:
	void preLoop() override;

	void renderImGui(const sdl::DeltaTime& deltaTime) override;

	void processEvent(const SDL_Event& windowEvent) override;

	void renderFrame(const sdl::DeltaTime& deltaTime, SDL_GPUTexture* swapchainTexture, SDL_GPUCommandBuffer* commandBuffer) override;

	void removeGamepad(SDL_JoystickID instanceId);

	std::function<void()> onSpacePressed_ = []() {
		spdlog::info("Space pressed");
	};
	int controllerEvent_ = 0;
	std::vector<sdl::GameController> gameControllers_;

	sdl::GpuBuffer myVertexBuffer_;
	sdl::GpuGraphicsPipeline myGraphicsPipeline_;
	sdl::GpuSampler sampler_;
	sdl::GpuTexture texture_;
	std::vector<sdl::Vertex> vertexes_;
	sdl::GpuTexture atlas_;

	sdl::Shader shader_;
	sdl::ImageAtlas imageAtlas_{600, 600};
};

#endif
