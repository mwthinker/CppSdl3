#ifndef CPPSDL3_SDL_SURFACE_H
#define CPPSDL3_SDL_SURFACE_H

#include "color.h"
#include "rect.h"

#include <SDL3/SDL_surface.h>

#include <string>

namespace sdl {

	class Font;

	class Surface {
	public:
		Surface() = default;
		~Surface();

		explicit Surface(const std::string& filename);

		Surface(int w, int h);

		Surface(int w, int h, Color color);

		Surface(std::string_view text, const Font& font, Color color);

		Surface(const Surface& surface) = delete;
		Surface& operator=(const Surface& surface) = delete;

		Surface(Surface&& other) noexcept;
		Surface& operator=(Surface&& other) noexcept;

		bool isLoaded() const noexcept;

		Size getSize() const noexcept;

		int getWidth() const noexcept;

		int getHeight() const noexcept;

		void blitSurface(const Surface& src, const Rect& rect);

	private:
		friend class Texture;
		friend class TextureAtlas;

		SDL_Surface* surface_ = nullptr;
	};

}

#endif
