#include "initsdl.h"

#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>

namespace sdl {

	InitSdl::InitSdl(Uint32 flags)
		: flags_{flags} {

		if (flags & SDL_INIT_VIDEO) {
			initSdl();
		}
		if (flags & SDL_INIT_GAMEPAD) {
			SDL_SetJoystickEventsEnabled(true);
		}
	}

	InitSdl::~InitSdl() {
		if (flags_ & SDL_INIT_GAMEPAD) {
			SDL_SetJoystickEventsEnabled(false);
		}
		if (flags_ & SDL_INIT_VIDEO) {
			SDL_Quit();
		}
	}

	void InitSdl::initSdl() {
		if (SDL_Init(flags_)) {
			spdlog::error("[sdl::InitSdl] Unable to init SDL: {}", SDL_GetError());
		}
	}

}
