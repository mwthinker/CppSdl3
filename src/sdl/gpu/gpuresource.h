#ifndef CPPSDL3_SDL_GPU_GPURESOURCE_H
#define CPPSDL3_SDL_GPU_GPURESOURCE_H

#include "../sdlexception.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>

#include <functional>

namespace sdl::gpu {

	/// @brief RAII wrapper for GPU resources in SDL. 
	/// Important! Must destruct/reset the resource before SDL_GPUDevice* is destroyed!
	/// @tparam Resource
	/// @tparam ReleaseFunc 
	template <typename Resource, auto ReleaseFunc>
	class GpuResource {
	public:
		GpuResource() = default;
		GpuResource(const GpuResource&) = delete;
		GpuResource& operator=(const GpuResource&) = delete;

		GpuResource(GpuResource&& other) noexcept
			: resource_(std::exchange(other.resource_, nullptr))
			, gpuDevice_(std::exchange(other.gpuDevice_, nullptr)) {
		}

		GpuResource& operator=(GpuResource&& other) noexcept {
			reset();

			resource_ = std::exchange(other.resource_, nullptr);
			gpuDevice_ = std::exchange(other.gpuDevice_, nullptr);
			return *this;
		}

		~GpuResource() {
			if (resource_ && gpuDevice_) {
				ReleaseFunc(gpuDevice_, resource_);
			} else if (resource_ && !gpuDevice_) {
				spdlog::warn("[GpuResource] Resource destroyed without an associated GpuContext! Potential leak!");
			}
		}

		Resource* get() const noexcept {
			return resource_;
		}

		bool isValid() const noexcept {
			return resource_ != nullptr;
		}

		template<typename CreateFunc, typename... Args>
		static GpuResource<Resource, ReleaseFunc> create(SDL_GPUDevice* gpuDevice, CreateFunc createFunc, Args&&... args) {
			GpuResource<Resource, ReleaseFunc> resource;
			resource.gpuDevice_ = gpuDevice;
			resource.resource_ = createFunc(gpuDevice, std::forward<Args>(args)...);
			if (!resource.resource_) {
				throw sdl::SdlException("Failed to create GPU resource");
			}
			return resource;
		}

		void reset() {
			if (resource_ && gpuDevice_) {
				ReleaseFunc(gpuDevice_, resource_);
				resource_ = nullptr;
				gpuDevice_ = nullptr;
			} else if (resource_) {
				spdlog::warn("[GpuResource] Resource released without an associated GpuContext! Potential leak!");
			}
		}

	private:
		Resource* resource_ = nullptr;
		SDL_GPUDevice* gpuDevice_ = nullptr;
	};

	using GpuSampler = GpuResource<SDL_GPUSampler, SDL_ReleaseGPUSampler>;
	using GpuTexture = GpuResource<SDL_GPUTexture, SDL_ReleaseGPUTexture>;
	using GpuBuffer = GpuResource<SDL_GPUBuffer, SDL_ReleaseGPUBuffer>;
	using GpuShader = GpuResource<SDL_GPUShader, SDL_ReleaseGPUShader>;
	using GpuGraphicsPipeline = GpuResource<SDL_GPUGraphicsPipeline, SDL_ReleaseGPUGraphicsPipeline>;
	using GpuComputePipeline = GpuResource<SDL_GPUComputePipeline, SDL_ReleaseGPUComputePipeline>;
	using GpuTransferBuffer = GpuResource<SDL_GPUTransferBuffer, SDL_ReleaseGPUTransferBuffer>;

	inline GpuSampler createSampler(SDL_GPUDevice* gpuDevice, const SDL_GPUSamplerCreateInfo& createInfo) {
		return GpuSampler::create(gpuDevice, SDL_CreateGPUSampler, &createInfo);
	}

	inline GpuTexture createTexture(SDL_GPUDevice* gpuDevice, const SDL_GPUTextureCreateInfo& createInfo) {
		return GpuTexture::create(gpuDevice, SDL_CreateGPUTexture, &createInfo);
	}

	inline GpuBuffer createBuffer(SDL_GPUDevice* gpuDevice, const SDL_GPUBufferCreateInfo& createInfo) {
		return GpuBuffer::create(gpuDevice, SDL_CreateGPUBuffer, &createInfo);
	}

	inline GpuShader createShader(SDL_GPUDevice* gpuDevice, const SDL_GPUShaderCreateInfo& createInfo) {
		return GpuShader::create(gpuDevice, SDL_CreateGPUShader, &createInfo);
	}

	inline GpuGraphicsPipeline createGraphicsPipeline(SDL_GPUDevice* gpuDevice, const SDL_GPUGraphicsPipelineCreateInfo& createInfo) {
		return GpuGraphicsPipeline::create(gpuDevice, SDL_CreateGPUGraphicsPipeline, &createInfo);
	}

	inline GpuComputePipeline createComputePipeline(SDL_GPUDevice* gpuDevice, const SDL_GPUComputePipelineCreateInfo& createInfo) {
		return GpuComputePipeline::create(gpuDevice, SDL_CreateGPUComputePipeline, &createInfo);
	}

	inline GpuTransferBuffer createTransferBuffer(SDL_GPUDevice* gpuDevice, const SDL_GPUTransferBufferCreateInfo& createInfo) {
		return GpuTransferBuffer::create(gpuDevice, SDL_CreateGPUTransferBuffer, &createInfo);
	}

}

#endif
