#ifndef CPPSDL3_SDL_GPU_GPUCONTEXT_H
#define CPPSDL3_SDL_GPU_GPUCONTEXT_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>

#include <functional>
#include <vector>

namespace sdl::gpu {

	class GpuContext {
	public:
		GpuContext() noexcept = default;
		~GpuContext();

		GpuContext(const GpuContext&) = delete;
		GpuContext& operator=(const GpuContext&) = delete;

		GpuContext(GpuContext&& other) = delete;
		GpuContext& operator=(GpuContext&& other) = delete;

		bool initialize(SDL_Window* window);

		void shutdown();

		SDL_GPUDevice* getGpuDevice() const noexcept;

		void flushDeferredReleases();

		/// @brief Process all the GPU resources that have been marked for deferred release.
		/// Calls undarlying SDL_Release_ function for each resource.
		template <typename T>
		void queueDeferredRelease(T* handle, std::function<void(SDL_GPUDevice*, T*)> typedReleaseFunc) {
			if (handle && gpuDevice_) {
				deferredReleaseQueue_.push_back(DeferredReleaseEntry{
					.releaseFunc = [typedReleaseFunc](SDL_GPUDevice* dev, void* h) {
						typedReleaseFunc(dev, static_cast<T*>(h));
					},
					.handle = static_cast<void*>(handle)
				});
			} else if (handle && !gpuDevice_) {
				spdlog::warn("[GpuContext] Attempted to queue a resource for release without an active GPU device.");
			}
		}

	private:
		struct DeferredReleaseEntry {
			std::function<void(SDL_GPUDevice*, void*)> releaseFunc;
			void* handle;
		};

		SDL_GPUDevice* gpuDevice_ = nullptr;
		SDL_Window* window_ = nullptr;
		std::vector<DeferredReleaseEntry> deferredReleaseQueue_;
	};

}

#endif
