#ifndef CPPSDL3_SDL_GPU_GPUUTIL_H
#define CPPSDL3_SDL_GPU_GPUUTIL_H

#include "sdlgpu.h"
#include "../imageatlas.h"

namespace sdl::gpu {
	
	[[nodiscard]]
	GpuTexture uploadSurface(GpuContext& context, SDL_Surface* surface);

	void blitToTexture(GpuContext& context, GpuTexture& texture, sdl::ImageAtlas& imageAtlas, SDL_Surface* surface, int border);

}

#endif
