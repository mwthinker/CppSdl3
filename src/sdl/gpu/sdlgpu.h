#ifndef CPPSDL3_SDL_SDLGPU_H
#define CPPSDL3_SDL_SDLGPU_H

#include "gpuresource.h"
#include "gpuutil.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_surface.h>

#include <concepts>
#include <span>
#include <ranges>

namespace sdl::gpu {

	template<typename T>
	concept VertexType = 
		std::is_standard_layout_v<T> &&
		std::is_trivially_copyable_v<T> &&
		alignof(T) == 16 &&
		std::is_class_v<T> && !std::is_reference_v<T>;

	/// \brief Compatible with UBO (Uniform Buffer Object) that requires std140 layout
#define VERTEX(name) struct alignas(16) name

	// Separate macro for validation that you use AFTER defining the struct
#define VERTEX_VALIDATE(name) \
	static_assert(sdl::gpu::VertexType<name>, "VERTEX(" #name ") does not satisfy VertexType requirements")

	template<typename Type>
	consteval void checkVertexType() {
		static_assert(sdl::gpu::VertexType<Type>, "Vertex must be compatible with UBO (Uniform Buffer Object) that requires std140 layout");
	}

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
