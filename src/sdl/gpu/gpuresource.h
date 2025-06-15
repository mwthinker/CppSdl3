#ifndef CPPSDL3_SDL_GPU_GPURESOURCE_H
#define CPPSDL3_SDL_GPU_GPURESOURCE_H

#include "gpucontext.h"
#include "../sdlexception.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>

#include <functional>

namespace sdl::gpu {

	template <typename Resource>
	class GpuResource {
	public:
		GpuResource() = default;
		GpuResource(const GpuResource&) = delete;
		GpuResource& operator=(const GpuResource&) = delete;

		GpuResource(GpuResource&& other) noexcept
			: handle_{std::exchange(other.handle_, nullptr)}
			, releaseFunc_{std::move(other.releaseFunc_)}
			, context_{std::exchange(other.context_, nullptr)} {
		}

		GpuResource& operator=(GpuResource&& other) noexcept {
			if (this != &other) {
				if (handle_ && context_) {
					context_->queueDeferredRelease(handle_, releaseFunc_);
				}

				handle_ = std::exchange(other.handle_, nullptr);
				releaseFunc_ = std::move(other.releaseFunc_);
				context_ = std::exchange(other.context_, nullptr);
			}
			return *this;
		}

		~GpuResource() {
			if (handle_ && context_) {
				context_->queueDeferredRelease(handle_, releaseFunc_);
			} else if (handle_ && !context_) {
				spdlog::warn("[GpuResource] Resource destroyed without an associated GpuContext!");
			}
		}

		Resource* get() const noexcept {
			return handle_;
		}

		bool isValid() const noexcept {
			return handle_ != nullptr;
		}

		template<typename CreateFunc, typename ReleaseFunc, typename... Args>
		static GpuResource<Resource> create(GpuContext& context, CreateFunc createFunc, ReleaseFunc releaseFunc, Args&&... args) {
			GpuResource<Resource> resource;
			resource.context_ = &context;
			resource.handle_ = createFunc(context.getGpuDevice(), std::forward<Args>(args)...);
			resource.releaseFunc_ = [releaseFunc](SDL_GPUDevice* dev, Resource* handle) {
				releaseFunc(dev, handle);
			};
			
			if (!resource.handle_) {
				throw sdl::SdlException("Failed to create GPU resource");
			}
			
			return resource;
		}

	private:
		Resource* handle_ = nullptr;
		std::function<void(SDL_GPUDevice*, Resource*)> releaseFunc_;
		GpuContext* context_ = nullptr;
	};

	using GpuSampler = GpuResource<SDL_GPUSampler>;
	using GpuTexture = GpuResource<SDL_GPUTexture>;
	using GpuBuffer = GpuResource<SDL_GPUBuffer>;
	using GpuShader = GpuResource<SDL_GPUShader>;
	using GpuGraphicsPipeline = GpuResource<SDL_GPUGraphicsPipeline>;
	using GpuComputePipeline = GpuResource<SDL_GPUComputePipeline>;
	using GpuTransferBuffer = GpuResource<SDL_GPUTransferBuffer>;

	inline GpuSampler createSampler(GpuContext& context, const SDL_GPUSamplerCreateInfo& createInfo) {
		return GpuSampler::create(context, SDL_CreateGPUSampler, SDL_ReleaseGPUSampler, &createInfo);
	}

	inline GpuTexture createTexture(GpuContext& context, const SDL_GPUTextureCreateInfo& createInfo) {
		return GpuTexture::create(context, SDL_CreateGPUTexture, SDL_ReleaseGPUTexture, &createInfo);
	}

	inline GpuBuffer createBuffer(GpuContext& context, const SDL_GPUBufferCreateInfo& createInfo) {
		return GpuBuffer::create(context, SDL_CreateGPUBuffer, SDL_ReleaseGPUBuffer, &createInfo);
	}

	inline GpuShader createShader(GpuContext& context, const SDL_GPUShaderCreateInfo& createInfo) {
		return GpuShader::create(context, SDL_CreateGPUShader, SDL_ReleaseGPUShader, &createInfo);
	}

	inline GpuGraphicsPipeline createGraphicsPipeline(GpuContext& context, const SDL_GPUGraphicsPipelineCreateInfo& createInfo) {
		return GpuGraphicsPipeline::create(context, SDL_CreateGPUGraphicsPipeline, SDL_ReleaseGPUGraphicsPipeline, &createInfo);
	}

	inline GpuComputePipeline createComputePipeline(GpuContext& context, const SDL_GPUComputePipelineCreateInfo& createInfo) {
		return GpuComputePipeline::create(context, SDL_CreateGPUComputePipeline, SDL_ReleaseGPUComputePipeline, &createInfo);
	}

	inline GpuTransferBuffer createTransferBuffer(GpuContext& context, const SDL_GPUTransferBufferCreateInfo& createInfo) {
		return GpuTransferBuffer::create(context, SDL_CreateGPUTransferBuffer, SDL_ReleaseGPUTransferBuffer, &createInfo);
	}

}

#endif
