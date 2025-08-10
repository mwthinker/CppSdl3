#ifndef CPPSDL3_SDL_SDLGPU_H
#define CPPSDL3_SDL_SDLGPU_H

#include "../sdlexception.h"

#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>

#include <concepts>
#include <span>
#include <ranges>
#include <memory>

namespace sdl::gpu {

	/// @brief Custom deleter for GPU resources that requires SDL_GPUDevice for cleanup
	/// @tparam Resource The GPU resource type
	/// @tparam ReleaseFunc The SDL release function
	template <typename Resource, auto ReleaseFunc>
	struct GpuResourceDeleter {
		SDL_GPUDevice* gpuDevice_ = nullptr;

		GpuResourceDeleter() noexcept = default;

		explicit GpuResourceDeleter(SDL_GPUDevice* gpuDevice) noexcept 
			: gpuDevice_{gpuDevice} {
		}

		void operator()(Resource* resource) const noexcept {
			if (resource && gpuDevice_) {
				ReleaseFunc(gpuDevice_, resource);
			} else if (resource && !gpuDevice_) {
				spdlog::warn("[GpuResource] Resource destroyed without an associated GpuDevice! Potential leak!");
			}
		}
	};

	// Type aliases for GPU resources using unique_ptr with custom deleters
	using GpuSampler = std::unique_ptr<SDL_GPUSampler, GpuResourceDeleter<SDL_GPUSampler, SDL_ReleaseGPUSampler>>;
	using GpuTexture = std::unique_ptr<SDL_GPUTexture, GpuResourceDeleter<SDL_GPUTexture, SDL_ReleaseGPUTexture>>;
	using GpuBuffer = std::unique_ptr<SDL_GPUBuffer, GpuResourceDeleter<SDL_GPUBuffer, SDL_ReleaseGPUBuffer>>;
	using GpuShader = std::unique_ptr<SDL_GPUShader, GpuResourceDeleter<SDL_GPUShader, SDL_ReleaseGPUShader>>;
	using GpuGraphicsPipeline = std::unique_ptr<SDL_GPUGraphicsPipeline, GpuResourceDeleter<SDL_GPUGraphicsPipeline, SDL_ReleaseGPUGraphicsPipeline>>;
	using GpuComputePipeline = std::unique_ptr<SDL_GPUComputePipeline, GpuResourceDeleter<SDL_GPUComputePipeline, SDL_ReleaseGPUComputePipeline>>;
	using GpuTransferBuffer = std::unique_ptr<SDL_GPUTransferBuffer, GpuResourceDeleter<SDL_GPUTransferBuffer, SDL_ReleaseGPUTransferBuffer>>;

	/// @brief Creates a GPU resource wrapped in unique_ptr with proper cleanup
	/// @tparam Resource The GPU resource type
	/// @tparam ReleaseFunc The SDL release function  
	/// @tparam CreateFunc The SDL create function type
	/// @tparam Args Variadic template for create function arguments
	/// @param gpuDevice The GPU device used for creation and cleanup
	/// @param createFunc The SDL create function
	/// @param arg Argument to pass to the create function
	/// @return unique_ptr managing the GPU resource
	template<typename Resource, auto ReleaseFunc, typename CreateFunc, typename Arg>
	auto createGpuResource(SDL_GPUDevice* gpuDevice, CreateFunc createFunc, Arg&& arg) {
		using DeleterType = GpuResourceDeleter<Resource, ReleaseFunc>;
		using ResourceUniquePtr = std::unique_ptr<Resource, DeleterType>;

		Resource* resource = createFunc(gpuDevice, std::forward<Arg>(arg));
		if (!resource) {
			throw sdl::SdlException{"Failed to create GPU resource"};
		}

		return ResourceUniquePtr{resource, DeleterType{gpuDevice}};
	}

	inline GpuSampler createSampler(SDL_GPUDevice* gpuDevice, const SDL_GPUSamplerCreateInfo& createInfo) {
		return createGpuResource<SDL_GPUSampler, SDL_ReleaseGPUSampler>(gpuDevice, SDL_CreateGPUSampler, &createInfo);
	}

	inline GpuTexture createTexture(SDL_GPUDevice* gpuDevice, const SDL_GPUTextureCreateInfo& createInfo) {
		return createGpuResource<SDL_GPUTexture, SDL_ReleaseGPUTexture>(gpuDevice, SDL_CreateGPUTexture, &createInfo);
	}

	inline GpuBuffer createBuffer(SDL_GPUDevice* gpuDevice, const SDL_GPUBufferCreateInfo& createInfo) {
		return createGpuResource<SDL_GPUBuffer, SDL_ReleaseGPUBuffer>(gpuDevice, SDL_CreateGPUBuffer, &createInfo);
	}

	inline GpuShader createShader(SDL_GPUDevice* gpuDevice, const SDL_GPUShaderCreateInfo& createInfo) {
		return createGpuResource<SDL_GPUShader, SDL_ReleaseGPUShader>(gpuDevice, SDL_CreateGPUShader, &createInfo);
	}

	inline GpuGraphicsPipeline createGraphicsPipeline(SDL_GPUDevice* gpuDevice, const SDL_GPUGraphicsPipelineCreateInfo& createInfo) {
		return createGpuResource<SDL_GPUGraphicsPipeline, SDL_ReleaseGPUGraphicsPipeline>(gpuDevice, SDL_CreateGPUGraphicsPipeline, &createInfo);
	}

	inline GpuComputePipeline createComputePipeline(SDL_GPUDevice* gpuDevice, const SDL_GPUComputePipelineCreateInfo& createInfo) {
		return createGpuResource<SDL_GPUComputePipeline, SDL_ReleaseGPUComputePipeline>(gpuDevice, SDL_CreateGPUComputePipeline, &createInfo);
	}

	inline GpuTransferBuffer createTransferBuffer(SDL_GPUDevice* gpuDevice, const SDL_GPUTransferBufferCreateInfo& createInfo) {
		return createGpuResource<SDL_GPUTransferBuffer, SDL_ReleaseGPUTransferBuffer>(gpuDevice, SDL_CreateGPUTransferBuffer, &createInfo);
	}

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
