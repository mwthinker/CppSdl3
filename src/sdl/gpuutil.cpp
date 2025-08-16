#include "gpuutil.h"
#include "gpu.h"
#include "util.h"
#include "sdlexception.h"
#include "color.h"
#include "imageatlas.h"

#include <stdexcept>

namespace sdl {

	GpuTexture uploadSurface(SDL_GPUDevice* gpuDevice, SDL_Surface* surface) {
		SdlSurface convertedSurfacePtr;
		if (surface->format != SDL_PIXELFORMAT_RGBA32) {
			convertedSurfacePtr.reset(SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32));
		}
		surface = convertedSurfacePtr ? convertedSurfacePtr.get() : surface;

		GpuTransferBuffer transferBuffer = createGpuTransferBuffer(
			gpuDevice,
			SDL_GPUTransferBufferCreateInfo{
				.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
				.size = static_cast<Uint32>(surface->w * surface->h * 4)
			}
		);

		mapTransferBuffer(
			gpuDevice,
			transferBuffer.get(),
			std::span{(Uint8*)surface->pixels, static_cast<size_t>(surface->w * surface->h * 4)},
			false
		);

		SDL_GPUTextureCreateInfo textureInfo{
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
			.width = static_cast<Uint32>(surface->w),
			.height = static_cast<Uint32>(surface->h),
			.layer_count_or_depth = 1,
			.num_levels = 1,
		};
		auto texture = createGpuTexture(gpuDevice, textureInfo);

		SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(gpuDevice);
		if (!uploadCmdBuf) {
			throw sdl::SdlException("Failed to acquire command buffer");
		}

		sdl::copyPass(uploadCmdBuf, [&](SDL_GPUCopyPass* copyPass) {
			SDL_GPUTextureTransferInfo transferInfo{
				.transfer_buffer = transferBuffer.get(),
				.offset = 0,
			};

			SDL_GPUTextureRegion textureRegion{
				.texture = texture.get(),
				.w = static_cast<Uint32>(surface->w),
				.h = static_cast<Uint32>(surface->h),
				.d = 1
			};

			SDL_UploadToGPUTexture(
				copyPass,
				&transferInfo,
				&textureRegion,
				false
			);
		});

		if (!SDL_SubmitGPUCommandBuffer(uploadCmdBuf)) {
			throw sdl::SdlException{"Failed to submit command buffer"};
		}

		return texture;
	}

	SDL_Rect blitToTexture(SDL_GPUDevice* gpuDevice, SDL_GPUTexture* texture, sdl::ImageAtlas& imageAtlas, SDL_Surface* surface, int border) {
		SdlSurface convertedSurfacePtr;;
		if (surface->format != SDL_PIXELFORMAT_RGBA32) {
			convertedSurfacePtr.reset(SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32));
		}
		surface = convertedSurfacePtr ? convertedSurfacePtr.get() : surface;

		auto rectOptional = imageAtlas.add(surface->w, surface->h, border);
		if (!rectOptional) {
			throw std::runtime_error{"Failed to blit surface to atlas"};
		}
		auto rect = *rectOptional;

		GpuTransferBuffer transferBuffer = createGpuTransferBuffer(
			gpuDevice,
			SDL_GPUTransferBufferCreateInfo{
				.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
				.size = static_cast<Uint32>(surface->w * surface->h * 4)
			}
		);

		mapTransferBuffer(
			gpuDevice,
			transferBuffer.get(),
			std::span{(Uint8*)surface->pixels, static_cast<size_t>(surface->w * surface->h * 4)},
			false
		);

		SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(gpuDevice);
		if (!uploadCmdBuf) {
			throw sdl::SdlException("Failed to acquire command buffer");
		}

		sdl::copyPass(uploadCmdBuf, [&](SDL_GPUCopyPass* copyPass) {
			SDL_GPUTextureTransferInfo transferInfo{
				.transfer_buffer = transferBuffer.get(),
				.offset = 0,
			};

			SDL_GPUTextureRegion textureRegion{
				.texture = texture,
				.x = static_cast<Uint32>(rect.x),
				.y = static_cast<Uint32>(rect.y),
				.w = static_cast<Uint32>(rect.w),
				.h = static_cast<Uint32>(rect.h),
				.d = 1
			};

			SDL_UploadToGPUTexture(
				copyPass,
				&transferInfo,
				&textureRegion,
				false
			);
		});

		if (!SDL_SubmitGPUCommandBuffer(uploadCmdBuf)) {
			throw sdl::SdlException{"Failed to submit command buffer"};
		}

		return rect;
	}

}
