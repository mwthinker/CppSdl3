#ifndef CPPSDL3_SDL_GPU_GPUUTIL_H
#define CPPSDL3_SDL_GPU_GPUUTIL_H

#include "sdlgpu.h"
#include "../imageatlas.h"

namespace sdl::gpu {
	
	[[nodiscard]]
	GpuTexture uploadSurface(SDL_GPUDevice* gpuDevice, SDL_Surface* surface);

	SDL_Rect blitToTexture(SDL_GPUDevice* gpuDevice, GpuTexture& texture, sdl::ImageAtlas& imageAtlas, SDL_Surface* surface, int border);

}

#endif
