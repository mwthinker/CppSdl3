#include "surface.h"

#include "font.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <tuple>

namespace sdl {

	namespace {

		SDL_Surface* createSurface(int w, int h, Color color) {
			auto surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
			SDL_FillSurfaceRect(surface, 0,
				SDL_MapSurfaceRGBA(surface, color.redByte(), color.greenByte(), color.blueByte(), color.alphaByte())
			);
			return surface;
		}

		SDL_Surface* createSurface(std::string_view text, TTF_Font* font, Color color) {
			if (font != nullptr) {
				SDL_Surface* argb = TTF_RenderText_Blended(font, text.data(), text.size(), color);
				SDL_Surface* rgba = SDL_ConvertSurface(argb, SDL_PIXELFORMAT_RGBA32);
				SDL_DestroySurface(argb);
				return rgba;
			}
			return nullptr;
		}

	}

	Surface::Surface(int w, int h) {
		assert(w > 0 && h > 0);
		surface_ = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
	}

	Surface::Surface(const std::string& filename) {
		surface_ = IMG_Load(filename.c_str());
		if (surface_ == nullptr) {
			spdlog::warn("[sdl::Surface] Image {} failed to be loaded: {}", filename, SDL_GetError());
		}
	}

	Surface::Surface(int w, int h, Color color) {
		assert(w > 0 && h > 0);
		surface_ = createSurface(w, h, color);
	}

	Surface::Surface(std::string_view text, const Font& font, Color color) {
		surface_ = createSurface(text, font.font_, color);
	}

	Surface::~Surface() {
		SDL_DestroySurface(surface_); // Safe to pass null.
	}

	Surface::Surface(Surface&& other) noexcept
		: surface_{std::exchange(other.surface_, nullptr)} {
	}

	Surface& Surface::operator=(Surface&& other) noexcept {
		SDL_DestroySurface(surface_); // Safe to pass null.
		surface_ = std::exchange(other.surface_, nullptr);
		return *this;
	}

	bool Surface::isLoaded() const noexcept {
		return surface_ != nullptr;
	}

	Size Surface::getSize() const noexcept {
		if (surface_ == nullptr) {
			return {0, 0};
		}
		return {surface_->w, surface_->h};
	}
	
	int Surface::getWidth() const noexcept {
		if (surface_ == nullptr) {
			return 0;
		}
		return surface_->w;
	}

	int Surface::getHeight() const noexcept {
		if (surface_ == nullptr) {
			return 0;
		}
		return surface_->h;
	}

	void Surface::blitSurface(const Surface& src, const Rect& rect) {
		auto newSurface = SDL_ConvertSurface(src.surface_, surface_->format);
		if (newSurface == nullptr) {
			spdlog::warn("[sdl::Surface] Failed to blit surface, during convert: {}", SDL_GetError());
			return;
		}
		SDL_Rect sdlRect = rect;
		if (SDL_BlitSurface(newSurface, 0, surface_, &sdlRect) != 0) {
			spdlog::warn("[sdl::Surface] Failed to blit surface: {}", SDL_GetError());
		}
	}
	
}
