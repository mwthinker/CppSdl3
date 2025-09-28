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

}

#endif
