#ifndef CPPSDL3_SDL_SDLEXCEPTION_H
#define CPPSDL3_SDL_SDLEXCEPTION_H

#include <string>
#include <stdexcept>

#include <SDL3/SDL_error.h>

namespace sdl {

	class SdlException : public std::runtime_error {
	public:
		explicit SdlException(const std::string& message)
			: std::runtime_error(message + ": " + SDL_GetError()) {
		}
		
		explicit SdlException(const char* message)
			: std::runtime_error(std::string(message) + ": " + SDL_GetError()) {
		}
		virtual ~SdlException() noexcept = default;
	};

}

#endif
