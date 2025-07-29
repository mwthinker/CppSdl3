#ifndef CPPSDL3_SDL_SDLGPU_H
#define CPPSDL3_SDL_SDLGPU_H

#include "gpuresource.h"
#include "gpuutil.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_surface.h>

#include <concepts>
#include <span>
#include <ranges>

/// \brief Compatible with UBO (Uniform Buffer Object) that requires std140 layout
#define VERTEX(name) struct alignas(16) name

namespace sdl::gpu {

	void copyPass(SDL_GPUCommandBuffer* commandBuffer, std::invocable<SDL_GPUCopyPass*> auto&& t) {
		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
		t(copyPass);
		SDL_EndGPUCopyPass(copyPass);
	}

	template<std::ranges::contiguous_range T>
	void mapTransferBuffer(SDL_GPUDevice* gpuDevice, GpuTransferBuffer& transferBuffer, const T& data, bool cycle = false) {
		auto bufferData = SDL_MapGPUTransferBuffer(gpuDevice, transferBuffer.get(), cycle);
		SDL_memcpy(bufferData, std::ranges::data(data), std::ranges::size(data) * sizeof(std::ranges::range_value_t<T>));
		SDL_UnmapGPUTransferBuffer(gpuDevice, transferBuffer.get());
	}

}

#endif
