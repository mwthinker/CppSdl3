#include "font.h"

#include <string>

#include <spdlog/spdlog.h>

namespace sdl {

	Font::~Font() {
		if (font_ != nullptr) {
			TTF_CloseFont(font_);
		}
	}

	Font::Font(const std::string& filename, float ptsize) {
		font_ = TTF_OpenFont(filename.c_str(), ptsize);

		if (font_ != nullptr) {
			ptsize_ = ptsize;
			TTF_SetFontHinting(font_, TTF_HINTING_LIGHT);
		} else {
			spdlog::error("[sdl::Font] Error font: {}", SDL_GetError());
		}
	}

	Font::Font(Font&& other) noexcept
		: font_{std::exchange(other.font_, nullptr)}
		, ptsize_{other.ptsize_} {}

	Font& Font::operator=(Font&& other) noexcept {
		if (font_ != nullptr) {
			TTF_CloseFont(font_);
		}
		font_ = std::exchange(other.font_, nullptr);
		ptsize_ = other.ptsize_;
		return *this;
	}

	bool operator==(const Font& left, const Font& right) noexcept {
		return left.font_ == right.font_;
	}

	bool operator!=(const Font& left, const Font& right) noexcept {
		return left.font_ != right.font_;
	}

	float Font::getCharacterSize() const noexcept {
		return ptsize_;
	}

	bool Font::isLoaded() const noexcept {
		return font_ != nullptr;
	}

}
