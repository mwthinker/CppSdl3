#ifndef CPPSDL3_SDL_GAMECONTROLLER_H
#define CPPSDL3_SDL_GAMECONTROLLER_H

#include <SDL3/SDL.h>

#include <string>

bool operator==(const SDL_GUID& guid1, const SDL_GUID& guid2);
bool operator!=(const SDL_GUID& guid1, const SDL_GUID& guid2);

namespace sdl {

	std::string guidToString(const SDL_GUID& guid);

	class GameController {
	public:
		static void loadGameControllerMappings(const std::string& file);

		static GameController addController(int index);

		static void removeController(GameController&& gameController);

		GameController() noexcept = default;
		~GameController();

		GameController(GameController&& other) noexcept;
		GameController& operator=(GameController&& other) noexcept;

		GameController(const GameController&) = delete;
		GameController& operator=(const GameController&) = delete;

		const char* getName() const;

		/// @brief Get the underlaying pointer (use with care!). Can be null.
		/// @return SDL_GameController pointer
		SDL_Gamepad* getSdlGameController() const;

		bool getButtonState(SDL_GamepadButton button) const;

		// Value ranging from -1 and 1. Trigger however range from 0 to 1.
		float getAxisState(SDL_GamepadAxis axis) const;

		bool isAttached() const;

		/// @brief Get the unique ID for the game controller for the time it is connected to the system,
		/// and is never reused for the lifetime of the application. If the joystick is
		///	disconnected and reconnected, it will get a new ID.
		/// 
		/// @return unique id for the game controller.
		SDL_JoystickID getInstanceId() const;

		/// @brief Get the stable unique GUID for the game controller
		/// @return GUID
		const SDL_GUID& getGuid() const;

	private:
		GameController(SDL_Gamepad* gameController, const SDL_GUID& guid) noexcept;

		void close();

		SDL_Gamepad* gameController_ = nullptr;
		SDL_GUID guid_{};
	};

	inline SDL_Gamepad* GameController::getSdlGameController() const {
		return gameController_;
	}

	inline bool GameController::getButtonState(SDL_GamepadButton button) const {
		return SDL_GetGamepadButton(gameController_, button) != 0;
	}

	inline float GameController::getAxisState(SDL_GamepadAxis axis) const {
		return static_cast<float>(SDL_GetGamepadAxis(gameController_, axis) / sizeof(Sint16));
	}

	inline bool GameController::isAttached() const {
		return SDL_GamepadConnected(gameController_);
	}

	inline SDL_JoystickID GameController::getInstanceId() const {
		return SDL_GetJoystickID(SDL_GetGamepadJoystick(gameController_));
	}

}

#endif
