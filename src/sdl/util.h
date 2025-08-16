#ifndef CPPSDL3_SDL_UTIL_H
#define CPPSDL3_SDL_UTIL_H

#include "imageatlas.h"

#include <chrono>
#include <memory>
#include <concepts>
#include <span>
#include <string>

#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_gamepad.h>

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
		if (!ptr) {
			throw std::runtime_error{"Not allowed to create SdlUniquePtr with a null pointer."};
		}
		return SdlUniquePtr<T, DestroyFn>{ptr};
	}

	using SdlSurface = SdlUniquePtr<SDL_Surface, SDL_DestroySurface>;
	using SdlGamepad = SdlUniquePtr<SDL_Gamepad, SDL_CloseGamepad>;

	/// @brief Takes ownership of an SDL_Surface and returns a unique pointer to it.
	/// @param surface
	/// @return A unique pointer to the SDL_Surface.
	inline SdlSurface createSdlSurface(SDL_Surface* surface) {
		return makeSdlUnique<SDL_Surface, SDL_DestroySurface>(surface);
	}

	/// @brief Takes ownership of an SDL_Gamepad and returns a unique pointer to it.
	/// @param gamepad
	/// @return A unique pointer to the SDL_Gamepad.
	inline SdlGamepad createSdlGamepad(SDL_Gamepad* gamepad) {
		return makeSdlUnique<SDL_Gamepad, SDL_CloseGamepad>(gamepad);
	}

	using Clock = std::chrono::high_resolution_clock;
	using DeltaTime = std::chrono::high_resolution_clock::duration;

	/// @brief Adds an image to the atlas and returns the rectangle where it was added. Throws an exception if the image cannot be added.
	/// @param atlas 
	/// @param surfaceAtlas 
	/// @param filename 
	/// @param border 
	/// @return SDL_Rect where the image was added in the atlas.
	SDL_Rect addImage(sdl::ImageAtlas& atlas, SDL_Surface* surfaceAtlas, const std::string& filename, int border = 0);

}

#endif
