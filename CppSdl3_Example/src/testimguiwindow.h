#ifndef TESTIMGUIWINDOW_H
#define TESTIMGUIWINDOW_H


#include <sdl/imguiwindow.h>
#include <sdl/gamecontroller.h>

#include <functional>
#include <vector>

class TestImGuiWindow : public sdl::ImGuiWindow {
public:
	TestImGuiWindow();

private:
	void initPreLoop() override;

	void imGuiUpdate(const sdl::DeltaTime& deltaTime) override;

	void imGuiPreUpdate(const sdl::DeltaTime& deltaTime) override;

	void imGuiEventUpdate(const SDL_Event& windowEvent) override;
};

#endif
