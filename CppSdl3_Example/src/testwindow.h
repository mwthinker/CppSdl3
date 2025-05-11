#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <sdl/window.h>
#include <sdl/gamecontroller.h>

#include <functional>
#include <vector>

class TestWindow : public sdl::Window {
public:
	TestWindow();

	void setSpaceFunction(std::function<void()>&& onSpacePressed) {
		onSpacePressed_ = std::move(onSpacePressed);
	}

private:
	void initPreLoop() override;

    void update(const sdl::DeltaTime& deltaTime) override;

	void eventUpdate(const SDL_Event& windowEvent) override;

	void resize(int w, int h);

	void removeGamepad(SDL_JoystickID instanceId);

	bool focus_ = true;
	int controllerEvent_ = 0;
	
	std::function<void()> onSpacePressed_;
	std::vector<sdl::GameController> gameControllers_;
};

#endif
