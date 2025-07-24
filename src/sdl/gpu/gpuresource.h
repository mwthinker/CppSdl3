#ifndef CPPSDL3_SDL_GPU_GPURESOURCE_H
#define CPPSDL3_SDL_GPU_GPURESOURCE_H

#include "gpucontext.h"
#include "../sdlexception.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>

#include <functional>

namespace sdl::gpu {

	/// @brief RAII wrapper for GPU resources in SDL. 
	/// Important! To destruct/reset the resource before the GpuContext is shutdown!
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
			, context_(std::exchange(other.context_, nullptr)) {
		}

		GpuResource& operator=(GpuResource&& other) noexcept {
			reset();

			resource_ = std::exchange(other.resource_, nullptr);
			context_ = std::exchange(other.context_, nullptr);
			return *this;
		}

		~GpuResource() {
			if (resource_ && context_) {
				ReleaseFunc(context_->getGpuDevice(), resource_);
			} else if (resource_ && !context_) {
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
		static GpuResource<Resource, ReleaseFunc> create(GpuContext& context, CreateFunc createFunc, Args&&... args) {
			GpuResource<Resource, ReleaseFunc> resource;
			resource.context_ = &context;
			resource.resource_ = createFunc(context.getGpuDevice(), std::forward<Args>(args)...);
			if (!resource.resource_) {
				throw sdl::SdlException("Failed to create GPU resource");
			}
			return resource;
		}

		void reset() {
			if (resource_ && context_) {
				ReleaseFunc(context_->getGpuDevice(), resource_);
				resource_ = nullptr;
				context_ = nullptr;
			} else if (resource_) {
				spdlog::warn("[GpuResource] Resource released without an associated GpuContext! Potential leak!");
			}
		}

	private:
		Resource* resource_ = nullptr;
		GpuContext* context_ = nullptr;
	};

	using GpuSampler = GpuResource<SDL_GPUSampler, SDL_ReleaseGPUSampler>;
	using GpuTexture = GpuResource<SDL_GPUTexture, SDL_ReleaseGPUTexture>;
	using GpuBuffer = GpuResource<SDL_GPUBuffer, SDL_ReleaseGPUBuffer>;
	using GpuShader = GpuResource<SDL_GPUShader, SDL_ReleaseGPUShader>;
	using GpuGraphicsPipeline = GpuResource<SDL_GPUGraphicsPipeline, SDL_ReleaseGPUGraphicsPipeline>;
	using GpuComputePipeline = GpuResource<SDL_GPUComputePipeline, SDL_ReleaseGPUComputePipeline>;
	using GpuTransferBuffer = GpuResource<SDL_GPUTransferBuffer, SDL_ReleaseGPUTransferBuffer>;

	inline GpuSampler createSampler(GpuContext& context, const SDL_GPUSamplerCreateInfo& createInfo) {
		return GpuSampler::create(context, SDL_CreateGPUSampler, &createInfo);
	}

	inline GpuTexture createTexture(GpuContext& context, const SDL_GPUTextureCreateInfo& createInfo) {
		return GpuTexture::create(context, SDL_CreateGPUTexture, &createInfo);
	}

	inline GpuBuffer createBuffer(GpuContext& context, const SDL_GPUBufferCreateInfo& createInfo) {
		return GpuBuffer::create(context, SDL_CreateGPUBuffer, &createInfo);
	}

	inline GpuShader createShader(GpuContext& context, const SDL_GPUShaderCreateInfo& createInfo) {
		return GpuShader::create(context, SDL_CreateGPUShader, &createInfo);
	}

	inline GpuGraphicsPipeline createGraphicsPipeline(GpuContext& context, const SDL_GPUGraphicsPipelineCreateInfo& createInfo) {
		return GpuGraphicsPipeline::create(context, SDL_CreateGPUGraphicsPipeline, &createInfo);
	}

	inline GpuComputePipeline createComputePipeline(GpuContext& context, const SDL_GPUComputePipelineCreateInfo& createInfo) {
		return GpuComputePipeline::create(context, SDL_CreateGPUComputePipeline, &createInfo);
	}

	inline GpuTransferBuffer createTransferBuffer(GpuContext& context, const SDL_GPUTransferBufferCreateInfo& createInfo) {
		return GpuTransferBuffer::create(context, SDL_CreateGPUTransferBuffer, &createInfo);
	}

}

#endif
