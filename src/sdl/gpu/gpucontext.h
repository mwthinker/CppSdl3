#ifndef CPPSDL3_SDL_GPU_GPUCONTEXT_H
#define CPPSDL3_SDL_GPU_GPUCONTEXT_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

namespace sdl::gpu {

	class GpuContext {
	public:
		GpuContext() noexcept = default;
		~GpuContext();

		GpuContext(const GpuContext&) = delete;
		GpuContext& operator=(const GpuContext&) = delete;

		GpuContext(GpuContext&& other) noexcept;
		GpuContext& operator=(GpuContext&& other) noexcept;

		bool initialize(SDL_Window* window);

		void shutdown();

		SDL_GPUDevice* getGpuDevice() const noexcept;

	private:
		SDL_GPUDevice* gpuDevice_ = nullptr;
		SDL_Window* window_ = nullptr;
	};

}

#endif
