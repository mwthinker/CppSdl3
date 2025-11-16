#ifndef CPPSDL3_SDL_GPUUTIL_H
#define CPPSDL3_SDL_GPUUTIL_H

#include "gpu.h"
#include "imageatlas.h"

#include <SDL3/SDL_surface.h>

namespace sdl {
	
	[[nodiscard]]
	GpuTexture uploadSurface(SDL_GPUDevice* gpuDevice, SDL_Surface* surface);

	[[nodiscard]]
	SDL_Rect blitToGpuTexture(SDL_GPUDevice* gpuDevice, SDL_GPUTexture* texture, sdl::ImageAtlas& imageAtlas, SDL_Surface* surface, int border);

	class Buffer {
	public:
		template <typename T>
		[[nodiscard]]
		SDL_GPUBuffer* get(SDL_GPUDevice* gpuDevice, SDL_GPUBufferUsageFlags flag, std::span<const T> data) {
			if (!buffer_.get() || bytes_ < data.size_bytes()) {
				bytes_ = data.size_bytes();

				SDL_GPUBufferCreateInfo vertexBufferInfo{
					.usage = flag,
					.size = static_cast<Uint32>(bytes_)
				};
				buffer_ = sdl::createGpuBuffer(gpuDevice, vertexBufferInfo);
			}
			return buffer_.get();
		}
		
		[[nodiscard]]
		SDL_GPUBuffer* get() {
			return buffer_.get();
		}

		void reset() {
			bytes_ = 0;
			buffer_.reset();
		}

	private:
		size_t bytes_ = 0;
		sdl::GpuBuffer buffer_;
	};

	class TransferBuffer {
	public:
		template <typename T>
		[[nodiscard]]
		SDL_GPUTransferBuffer* get(SDL_GPUDevice* gpuDevice, std::span<const T> data, bool cycle = false) {
			if (!transferBuffer_.get() || bytes_ < data.size_bytes()) {
				bytes_ = data.size_bytes();

				SDL_GPUTransferBufferCreateInfo transferInfo{
					.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
					.size = static_cast<Uint32>(bytes_)
				};
				transferBuffer_ = sdl::createGpuTransferBuffer(gpuDevice, transferInfo);
			}
			sdl::mapGpuTransferBuffer(gpuDevice, transferBuffer_.get(), data, cycle);
			return transferBuffer_.get();
		}

		[[nodiscard]]
		SDL_GPUTransferBuffer* get() {
			return transferBuffer_.get();
		}

		void reset() {
			bytes_ = 0;
			transferBuffer_.reset();
		}

	private:
		size_t bytes_ = 0;
		sdl::GpuTransferBuffer transferBuffer_;
	};

}

#endif
