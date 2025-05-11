#ifndef CPPSDL3_SDL_RECT_H
#define CPPSDL3_SDL_RECT_H

#include <SDL3/SDL_rect.h>

namespace sdl {

	using Rect = SDL_Rect;

	struct Position {
		int x, y;
	};

	struct Size {
		int width, height;
	};

}

#endif
