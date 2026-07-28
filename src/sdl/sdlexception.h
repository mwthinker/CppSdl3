#ifndef CPPSDL3_SDL_SDLEXCEPTION_H
#define CPPSDL3_SDL_SDLEXCEPTION_H

#include <SDL3/SDL_error.h>

#include <fmt/format.h>

#include <stdexcept>
#include <source_location>

namespace sdl {

	class SdlException : public std::runtime_error {
	public:
		explicit SdlException(const std::string& message, std::source_location location = std::source_location::current())
			: std::runtime_error(fmt::format(
				"{}\n{} ({}:{}:{}): {}",
				message,
				location.file_name(),
				location.line(),
				location.column(),
				location.function_name(),
				SDL_GetError()))
		{
		}

		~SdlException() noexcept override = default;

	};

}

#endif
