#include "testwindow.h"

#include <sdl/window.h>
#include <sdl/gamecontroller.h>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

namespace {

	void printGameControllerButton(Uint8 button) {
		switch (button) {
			case SDL_GAMEPAD_BUTTON_SOUTH:
				fmt::println("SDL_GAMEPAD_BUTTON_SOUTH");
				break;
			case SDL_GAMEPAD_BUTTON_EAST:
				fmt::println("SDL_GAMEPAD_BUTTON_EAST");
				break;
			case SDL_GAMEPAD_BUTTON_WEST:
				fmt::println("SDL_GAMEPAD_BUTTON_WEST");
				break;
			case SDL_GAMEPAD_BUTTON_NORTH:
				fmt::println("SDL_GAMEPAD_BUTTON_NORTH");
				break;
			case SDL_GAMEPAD_BUTTON_BACK:
				fmt::println("SDL_GAMEPAD_BUTTON_BACK");
				break;
			case SDL_GAMEPAD_BUTTON_GUIDE:
				fmt::println("SDL_GAMEPAD_BUTTON_GUIDE");
				break;
			case SDL_GAMEPAD_BUTTON_START:
				fmt::println("SDL_GAMEPAD_BUTTON_START");
				break;
			case SDL_GAMEPAD_BUTTON_LEFT_STICK:
				fmt::println("SDL_GAMEPAD_BUTTON_LEFT_STICK");
				break;
			case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
				fmt::println("SDL_GAMEPAD_BUTTON_RIGHT_STICK");
				break;
			case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
				fmt::println("SDL_GAMEPAD_BUTTON_LEFT_SHOULDER");
				break;
			case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
				fmt::println("SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER");
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_UP:
				fmt::println("SDL_GAMEPAD_BUTTON_DPAD_UP");
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
				fmt::println("SDL_GAMEPAD_BUTTON_DPAD_DOWN");
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
				fmt::println("SDL_GAMEPAD_BUTTON_DPAD_LEFT");
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
				fmt::println("SDL_GAMEPAD_BUTTON_DPAD_RIGHT");
				break;
			case SDL_GAMEPAD_BUTTON_COUNT:
				fmt::println("SDL_GAMEPAD_BUTTON_COUNT");
				break;
		}
	}

}

TestWindow::TestWindow() {
	sdl::Window::setSize(512, 512);
	sdl::Window::setTitle("Test");
	sdl::Window::setIcon("tetris.bmp");
}

void TestWindow::update(const sdl::DeltaTime& deltaTime) {

}

void TestWindow::eventUpdate(const SDL_Event& windowEvent) {
	switch (windowEvent.type) {
		case SDL_EVENT_QUIT:
			quit();
			break;
		case SDL_EVENT_MOUSE_WHEEL: {
			auto add = windowEvent.wheel.y * 5;
			auto [w, h] = getSize();
			fmt::println("{} add = {}", windowEvent.wheel.which, add);
			setSize(static_cast<int>(add +  w), static_cast<int>(add + h));
			break;
		}
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			if (windowEvent.window.windowID == getId()) {
				quit();
			}
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			fmt::println("SDL_EVENT_WINDOW_RESIZED");
			resize(windowEvent.window.data1, windowEvent.window.data2);
			break;
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			fmt::println("SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED");
			resize(windowEvent.window.data1, windowEvent.window.data2);
			break;
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			if (windowEvent.window.windowID == getId()) {
				focus_ = true;
			}
			break;
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			if (windowEvent.window.windowID == getId()) {
				focus_ = false;
			}
			break;
		case SDL_EVENT_KEY_DOWN:
			switch (windowEvent.key.key) {
				case SDLK_C:
					for (auto& gamepad : gameControllers_) {
						fmt::println("{}", gamepad.getName());
					}
					break;
				case SDLK_SPACE:
					if (onSpacePressed_) {
						onSpacePressed_();
					}
					break;
				case SDLK_ESCAPE:
					if (focus_) {
						quit();
					}
					break;
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (windowEvent.button.button == SDL_BUTTON_LEFT) {
				if (windowEvent.button.clicks == 2) {
					setFullScreen(!isFullScreen());
				}
			}
			fmt::println("DOWN");
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			fmt::println("UP");
			break;
		case SDL_EVENT_GAMEPAD_ADDED:
			fmt::println("\nSDL_CONTROLLERDEVICEADDED");
			fmt::println("ControllerEvent: {}", ++controllerEvent_);
			fmt::println("Timestamp: {}", windowEvent.cdevice.timestamp);
			fmt::println("Type: {}", (int) windowEvent.cdevice.type);
			fmt::println("Which (Joystick index): {}", windowEvent.cdevice.which);
			gameControllers_.push_back(sdl::GameController::addController(windowEvent.cdevice.which));
			break;
		case SDL_EVENT_GAMEPAD_REMAPPED:
			fmt::println("SDL_CONTROLLERDEVICEREMOVED");
			fmt::println("ControllerEvent: {}", ++controllerEvent_);
			fmt::println("Timestamp: {}", windowEvent.cdevice.timestamp);
			fmt::println("Type: {}", (int) windowEvent.cdevice.type);
			fmt::println("Which (InstanceId): {}", windowEvent.cdevice.which);
			break;
		case SDL_EVENT_GAMEPAD_REMOVED:
			fmt::println("\nSDL_CONTROLLERDEVICEREMOVED");
			fmt::println("ControllerEvent: {}", ++controllerEvent_);
			fmt::println("Timestamp: {}", windowEvent.cdevice.timestamp);
			fmt::println("Type: {}", (int) windowEvent.cdevice.type);
			fmt::println("Which (InstanceId): {}", windowEvent.cdevice.which);
			removeGamepad(windowEvent.cdevice.which);
			break;
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			printGameControllerButton(windowEvent.gbutton.button);
			fmt::println("SDL_CONTROLLERBUTTONDOWN");
			fmt::println("ControllerEvent: {}", ++controllerEvent_);
			fmt::println("Timestamp: {}", windowEvent.gbutton.timestamp);
			fmt::println("Type: {}", (int) windowEvent.gbutton.type);
			fmt::println("Which: {}", windowEvent.gbutton.which);
			break;
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			break;
		case SDL_EVENT_GAMEPAD_AXIS_MOTION:
			switch (windowEvent.gaxis.axis) {
				case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
					if (windowEvent.gaxis.value > 15000) {
						fmt::println("SDL_CONTROLLER_AXIS_TRIGGERLEFT");
					}
					break;
				case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
					if (windowEvent.gaxis.value > 15000) {
						fmt::println("SDL_CONTROLLER_AXIS_TRIGGERRIGHT");
					}
					break;
			}

			break;
	}
}

void TestWindow::resize(int w, int h) {
	//gl::glViewport(0, 0, w, h);
	//shader_->setProjectionMatrixU(mw::getOrthoProjectionMatrix44<GLfloat>(0, (GLfloat) w, 0, (GLfloat) h));
	//shader_->setProjectionMatrixU(mw::getOrthoProjectionMatrix44<GLfloat>(0, (GLfloat) w, 0, (GLfloat) h));
	//shader_->setProjectionMatrix(glm::ortho(-0.5f * sprite_.getWidth(), 0.5f * sprite_.getHeight(), -0.5f * sprite_.getHeight(), 0.5f * sprite_.getHeight()));
}

void TestWindow::removeGamepad(SDL_JoystickID instanceId) {
	auto it = std::find_if(gameControllers_.begin(), gameControllers_.end(), [instanceId](const sdl::GameController& gameController) {
		return gameController.getInstanceId() == instanceId;
	});

	if (it != gameControllers_.end()) {
		gameControllers_.erase(it);
		spdlog::info("Gamepad removed: {}", instanceId);
	} else {
		spdlog::warn("Gamepad failed to be removed: {}", instanceId);
	}
}

void TestWindow::initPreLoop() {
	setLoopSleepingTime(std::chrono::milliseconds{10});
	resize(getWidth(), getHeight());

	sdl::GameController::loadGameControllerMappings("gamecontrollerdb.txt");
	setHitTestCallback([](const SDL_Point&) {
		return SDL_HITTEST_DRAGGABLE;
	});
}
