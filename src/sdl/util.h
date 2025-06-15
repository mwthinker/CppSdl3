#ifndef CPPSDL3_SDL_UTIL_H
#define CPPSDL3_SDL_UTIL_H

#include "imageatlas.h"

#include <chrono>
#include <memory>
#include <concepts>
#include <span>
#include <string>

#include <SDL3_image/SDL_image.h>

namespace sdl {

	/// @brief A function object that deletes a pointer using a custom destructor function.
	/// @tparam T type of the pointer to be deleted.
	/// @tparam DestroyFn pointer to a function that destroys an object of type T*.
	template <typename T, void(*DestroyFn)(T*)>
	struct SDLDeleter {
		void operator()(T* ptr) const {
			if (ptr) DestroyFn(ptr);
		}
	};

	/// @brief A unique pointer for SDL resources with a custom deleter.
	/// @tparam T type of the SDL resource.
	/// @tparam DestroyFn function pointer to the SDL resource's destructor
	template <typename T, void(*DestroyFn)(T*)>
	using SdlUniquePtr = std::unique_ptr<T, SDLDeleter<T, DestroyFn>>;

	/// @brief Creates a unique pointer for an SDL resource with a custom deleter.
	/// @tparam T type of the SDL resource.
	/// @tparam DestroyFn function pointer to the SDL resource's destructor
	/// @param ptr pointer to the SDL resource to manage.
	/// @return A unique pointer to the SDL resource.
	template <typename T, void(*DestroyFn)(T*)>
	SdlUniquePtr<T, DestroyFn> makeSdlUnique(T* ptr) {
		return SdlUniquePtr<T, DestroyFn>{ptr};
	}

	using SdlSurfacePtr = SdlUniquePtr<SDL_Surface, SDL_DestroySurface>;
	using SdlGamepadPtr = SdlUniquePtr<SDL_Gamepad, SDL_CloseGamepad>;

	/// @brief Takes ownership of an SDL_Surface and returns a unique pointer to it.
	/// @param surface
	/// @return A unique pointer to the SDL_Surface.
	inline SdlSurfacePtr createSdlSurface(SDL_Surface* surface) {
		return makeSdlUnique<SDL_Surface, SDL_DestroySurface>(surface);
	}

	/// @brief Takes ownership of an SDL_Gamepad and returns a unique pointer to it.
	/// @param gamepad
	/// @return A unique pointer to the SDL_Gamepad.
	inline SdlGamepadPtr createSdlGamepad(SDL_Gamepad* gamepad) {
		return makeSdlUnique<SDL_Gamepad, SDL_CloseGamepad>(gamepad);
	}

	using Clock = std::chrono::high_resolution_clock;
	using DeltaTime = std::chrono::high_resolution_clock::duration;

	SdlSurfacePtr createSurfaceAtlas(std::span<std::string> files, int width, int height, int border = 0);

}

#endif
