#ifndef CPPSDL3_SDL_UTIL_IMAGEATLAS_H
#define CPPSDL3_SDL_UTIL_IMAGEATLAS_H

#include <SDL3/SDL_rect.h>

#include <memory>
#include <optional>

namespace sdl {

	// The packing algorithm is from http://www.blackpawn.com/texts/lightmaps/default.html.
	class ImageAtlas {
	public:
		ImageAtlas();

		ImageAtlas(int width, int height);
		
		std::optional<SDL_Rect> add(int width, int height, int border = 0);

		int getWidth() const noexcept;

		int getHeight() const noexcept;

	private:
		struct Node {
			Node(const SDL_Rect& rect);

			Node* insert(const SDL_Rect& surface, int border);

			bool image = false;
			std::unique_ptr<Node> left_;
			std::unique_ptr<Node> right_;
			
			SDL_Rect rect_{};
		};

		Node root_{SDL_Rect{0, 0, 2048, 2048}};
	};

}

#endif
