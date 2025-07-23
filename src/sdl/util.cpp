#include "util.h"
#include "imageatlas.h"
#include "color.h"
#include "sdlexception.h"

#include <SDL3_image/SDL_image.h>

namespace sdl {

	namespace {

		SDL_Surface* createSurface(int w, int h, sdl::Color color) {
			auto s = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
			SDL_FillSurfaceRect(s, nullptr, color.toImU32());
			return s;
		}

	}

	SDL_Rect addImage(sdl::ImageAtlas& atlas, SDL_Surface* surfaceAtlas, const std::string& filename, int border) {
		auto surface = sdl::createSdlSurface(IMG_Load(filename.c_str()));
		if (!surface) {
			throw sdl::SdlException{"Failed to load surface from file '{}'", filename};
		}
		auto rectOptional = atlas.add(surface->w, surface->h, border);
		if (!rectOptional) {
			throw std::runtime_error{fmt::format("Surface did not fit in atlas, {}x{}, format {}",
				surface->w, surface->h, SDL_GetPixelFormatName(surface->format))};
		}

		auto rectDst = *rectOptional;
		if (SDL_BlitSurface(surface.get(), nullptr, surfaceAtlas, &rectDst)) {
			return rectDst;
		} else {
			throw sdl::SdlException{"Failed to blit surface to atlas '{}'", filename};
		}
	}

}
