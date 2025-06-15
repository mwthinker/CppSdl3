#include "util.h"
#include "imageatlas.h"
#include "color.h"

#include <spdlog/spdlog.h>

namespace sdl {

	namespace {

		SDL_Surface* createSurface(int w, int h, sdl::Color color) {
			auto s = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
			SDL_FillSurfaceRect(s, nullptr, color.toImU32());
			return s;
		}

	}

	SdlSurfacePtr createSurfaceAtlas(std::span<std::string> files, int width, int height, int border) {
		ImageAtlas imageAtlas{width, height};
		auto surfaceAtlas = sdl::createSdlSurface(createSurface(width, height, sdl::color::Transparent));

		for (const auto& file : files) {
			auto surface = sdl::createSdlSurface(IMG_Load(file.c_str()));
			if (!surface) {
				spdlog::warn("Failed to load surface from file '{}': {}", file, SDL_GetError());
				continue;
			}
			auto rectOptional = imageAtlas.add(surface->w, surface->h, border);
			if (!rectOptional) {
				spdlog::warn("Failed to add surface to atlas: {}x{}, format: {}",
					surface->w, surface->h, SDL_GetPixelFormatName(surface->format));
				continue;
			}
			
			auto rect = *rectOptional;
			if (SDL_BlitSurface(surface.get(), nullptr, surfaceAtlas.get(), &rect)) {
				spdlog::info("Added surface to atlas: {}x{}, format: {}, at position: {},{}",
					surface->w, surface->h, SDL_GetPixelFormatName(surface->format), rect.x, rect.y);
			} else {
				spdlog::error("Failed to blit surface to atlas: {}x{}, format: {}",
					surface->w, surface->h, SDL_GetPixelFormatName(surface->format));
			}
		}

		return surfaceAtlas;
	}

}
