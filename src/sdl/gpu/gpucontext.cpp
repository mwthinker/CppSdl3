#include "gpucontext.h"
#include "../sdlexception.h"

#include <SDL3/SDL.h>

#include <spdlog/spdlog.h>

namespace sdl::gpu {
	
	GpuContext::~GpuContext() {
		flushDeferredReleases();
		if (gpuDevice_) {
			if (!SDL_WaitForGPUIdle(gpuDevice_)) {
				spdlog::error("[GpuContext] SDL_WaitForGPUIdle failed: {}", SDL_GetError());
			}
			SDL_DestroyGPUDevice(gpuDevice_);
			gpuDevice_ = nullptr;
		}
		window_ = nullptr;
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
		window_ = window;

		//SDL_GpuEnumerateBackends();
		//SDL_GpuIsShaderFormatSupported
		for (int i = 0; i < SDL_GetNumGPUDrivers(); ++i) {
			spdlog::info("[GpuContext] Preferred GPU driver: {}", SDL_GetGPUDriver(i));
		}
		int size = SDL_GetNumGPUDrivers();
		gpuDevice_ = gpuDevice_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB, true, nullptr);
		if (!gpuDevice_) {
			spdlog::error("[GpuContext] Failed to create GPU device: {}", SDL_GetError());
			return false;
		}
		if (!SDL_ClaimWindowForGPUDevice(gpuDevice_, window_)) {
			spdlog::error("[GpuContext] Failed to claim window for GPU device: {}", SDL_GetError());
			SDL_DestroyGPUDevice(gpuDevice_);
			gpuDevice_ = nullptr;
			return false;
		}

		return true;
	}

	void GpuContext::shutdown() {
		if (!gpuDevice_) return;

		// Wait for all commands to complete
		SDL_WaitForGPUIdle(gpuDevice_);

		// Release any resources queued for deferred release
		flushDeferredReleases();

		// Unclaim the window from the device
		if (window_ && gpuDevice_) {
			SDL_ReleaseWindowFromGPUDevice(gpuDevice_, window_);
		}
		SDL_DestroyGPUDevice(gpuDevice_);
		
		gpuDevice_ = nullptr;
		window_ = nullptr;
	}

	SDL_GPUDevice* GpuContext::getGpuDevice() const noexcept {
		return gpuDevice_;
	}

	void GpuContext::flushDeferredReleases() {
		for (const auto& entry : deferredReleaseQueue_) {
			if (entry.handle && gpuDevice_) {
				entry.releaseFunc(gpuDevice_, entry.handle);
			}
		}
		deferredReleaseQueue_.clear();
	}

}
