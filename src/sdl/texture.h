#ifndef CPPSDL2_SDL_TEXTURE_H
#define CPPSDL2_SDL_TEXTURE_H

#include "rect.h"
#include "surface.h"

#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>

#include <type_traits>

namespace sdl {

	/*
	class Texture {
	public:
		friend class TextureAtlas;

		Texture() = default;
		~Texture();

		Texture(const Texture& texture) = delete;
		Texture& operator=(const Texture& texture) = delete;

		Texture(Texture&& texture) noexcept;
		Texture& operator=(Texture&& texture) noexcept;
		
		void bind();
		
		void texImage(const Surface& surface);

		void texImage(const Surface& surface, std::invocable auto&& filter);

		void texSubImage(const Surface& surface, const Rect& dst);

		void generate();
		
		bool isValid() const noexcept;

		friend bool operator==(const Texture& left, const Texture& right) noexcept;
		
		friend bool operator!=(const Texture& left, const Texture& right) noexcept;

		operator gl::GLuint() const noexcept {
			return texture_;
		}

	private:
		static gl::GLenum surfaceFormat(SDL_Surface* surface);

		gl::GLuint texture_{};
	};

	inline bool operator==(const Texture& left, const Texture& right) noexcept {
		return left.texture_ == right.texture_;
	}

	inline bool operator!=(const Texture& left, const Texture& right) noexcept {
		return left.texture_ != right.texture_;
	}

	inline void Texture::texImage(const Surface& surface) {
		texImage(surface, []() {
			gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
			gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);
		});
	}

	void Texture::texImage(const Surface& surface, std::invocable auto&& filter) {
		if (!surface.isLoaded()) {
			spdlog::debug("[sdl::Texture] Failed to bind, must be loaded first");
			return;
		}

		if (!isValid()) {
			spdlog::debug("[sdl::Texture] Failed to bind, must be generated first");
			return;
		}

		auto format = surfaceFormat(surface.surface_);
		if (format == 0) {
			spdlog::debug("[sdl::Texture] Failed BytesPerPixel, must be 1, 3 or 4. Is: {}", surface.surface_->format->BytesPerPixel);
			return;
		}

		gl::glBindTexture(gl::GL_TEXTURE_2D, texture_);
		filter();
		gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, format,
			surface.surface_->w, surface.surface_->h,
			0,
			format,
			gl::GL_UNSIGNED_BYTE,
			surface.surface_->pixels
		);
	}

	*/

}

#endif
