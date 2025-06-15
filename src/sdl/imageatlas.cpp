#include "imageatlas.h"

namespace sdl {

	ImageAtlas::ImageAtlas() = default;

	ImageAtlas::ImageAtlas(int width, int height)
		: root_{SDL_Rect{0, 0, width, height}} {
	}

	std::optional<SDL_Rect> ImageAtlas::add(int width, int height, int border) {
		if (width > root_.rect_.w || height > root_.rect_.h) {
			// Image to large!
			return std::nullopt;
		}
		auto node = root_.insert(SDL_Rect{0, 0, width, height}, border);
		if (!node) {
			// // Not enough image space to insert image.
			return std::nullopt;
		}

		// Only when atlas is not full.
		SDL_Rect rect = node->rect_;
		rect.w -= 2 * border;
		rect.h -= 2 * border;
		rect.x += border;
		rect.y += border;
		return std::make_optional<SDL_Rect>(rect);
	}

	int ImageAtlas::getWidth() const noexcept {
		return root_.rect_.w;
	}

	int ImageAtlas::getHeight() const noexcept {
		return root_.rect_.h;
	}


	ImageAtlas::Node::Node(const SDL_Rect& rect)
		: rect_{rect} {
	}

	ImageAtlas::Node* ImageAtlas::Node::insert(const SDL_Rect& surface, int border) {
		// Is not a leaf!
		if (left_ != nullptr && right_ != nullptr) {
			if (auto node = left_->insert(surface, border); node != nullptr) {
				// Image inserted.
				return node;
			}
			// Image did not fit, try the other node.
			return right_->insert(surface, border);
		} else {
			if (image) {
				// Node is already filled!
				return nullptr;
			}

			if (surface.w + 2 * border > rect_.w || surface.h + 2 * border > rect_.h) {
				// Image to large!
				return nullptr;
			}

			// Fits perfectly?
			if (surface.w + 2 * border == rect_.w && surface.h + 2 * border == rect_.h) {
				image = true;
				return this;
			}

			// Split the node in half.
			if (rect_.w - surface.w < rect_.h - surface.h) { // Split vertical.
				left_ = std::make_unique<Node>(SDL_Rect{rect_.x, rect_.y,
					rect_.w, surface.h + 2 * border}); // Up.

				right_ = std::make_unique<Node>(SDL_Rect{rect_.x, rect_.y + surface.h + 2 * border,
					rect_.w, rect_.h - surface.h - 2 * border}); // Down.

			} else { // Split horizontal.
				left_ = std::make_unique<Node>(SDL_Rect{rect_.x, rect_.y,
					surface.w + 2 * border, rect_.h}); // Left.

				right_ = std::make_unique<Node>(SDL_Rect{rect_.x + surface.w + 2 * border, rect_.y,
					rect_.w - surface.w - 2 * border, rect_.h}); // Right.
			}

			// Insert the image in the left node.
			return left_->insert(surface, border);
		}
	}

}
