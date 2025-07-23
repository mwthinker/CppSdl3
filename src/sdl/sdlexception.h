#ifndef CPPSDL3_SDL_SDLEXCEPTION_H
#define CPPSDL3_SDL_SDLEXCEPTION_H

#include <fmt/core.h>
#include <stdexcept>

#include <SDL3/SDL_error.h>

namespace sdl {

	class SdlException : public std::runtime_error {
	public:
		template <typename... Args>
		explicit SdlException(fmt::format_string<Args...> fmtStr, Args&&... args)
			: std::runtime_error{fmt::format("{}: {}", fmt::format(fmtStr, std::forward<Args>(args)...), SDL_GetError())} {
		}

		~SdlException() noexcept override = default;
	};

}

#endif
