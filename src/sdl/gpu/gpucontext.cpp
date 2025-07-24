#include "gpucontext.h"
#include "../sdlexception.h"

#include <SDL3/SDL.h>

#include <spdlog/spdlog.h>

namespace sdl::gpu {
	
	GpuContext::~GpuContext() {
		shutdown();
	}

	GpuContext::GpuContext(GpuContext&& other) noexcept {
		shutdown();

		gpuDevice_ = std::exchange(other.gpuDevice_, nullptr);
		window_ = std::exchange(other.window_, nullptr);
	}

	GpuContext& GpuContext::operator=(GpuContext&& other) noexcept {
		shutdown();

		gpuDevice_ = std::exchange(other.gpuDevice_, nullptr);
		window_ = std::exchange(other.window_, nullptr);
		return *this;
	}

	bool GpuContext::initialize(SDL_Window* window) {
		if (window_ != nullptr || gpuDevice_ != nullptr) {
			spdlog::warn("[GpuContext] Already initialized or in the process of being initialized.");
			return false;
		}

		if (!window) {
			spdlog::error("[GpuContext] Cannot initialize with a null window.");
			return false;
		}

		int driversNbr = SDL_GetNumGPUDrivers();
		for (int i = 0; i < driversNbr; ++i) {
			spdlog::info("[GpuContext] Preferred GPU driver: {}", SDL_GetGPUDriver(i));
		}

		gpuDevice_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL, true, nullptr);
		if (!gpuDevice_) {
			spdlog::error("[GpuContext] Failed to create GPU device: {}", SDL_GetError());
			return false;
		}

		if (!SDL_ClaimWindowForGPUDevice(gpuDevice_, window)) {
			spdlog::error("[GpuContext] Failed to claim window for GPU device: {}", SDL_GetError());
			SDL_DestroyGPUDevice(gpuDevice_);
			gpuDevice_ = nullptr;
			return false;
		}

		window_ = window;
		return true;
	}

	void GpuContext::shutdown() {
		if (!gpuDevice_) return;

		SDL_WaitForGPUIdle(gpuDevice_);

		if (window_) {
			SDL_ReleaseWindowFromGPUDevice(gpuDevice_, window_);
		}
		SDL_DestroyGPUDevice(gpuDevice_);

		gpuDevice_ = nullptr;
		window_ = nullptr;
	}

	SDL_GPUDevice* GpuContext::getGpuDevice() const noexcept {
		return gpuDevice_;
	}

}
