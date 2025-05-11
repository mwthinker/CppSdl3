#include "testimguiwindow.h"

#include <sdl/imguiauxiliary.h>

#include <spdlog/spdlog.h>

TestImGuiWindow::TestImGuiWindow() {
	sdl::Window::setSize(512, 512);
	sdl::Window::setTitle("Test");
	sdl::Window::setIcon("tetris.bmp");
	sdl::ImGuiWindow::setShowDemoWindow(true);
	sdl::ImGuiWindow::setShowColorWindow(true);
}

void TestImGuiWindow::imGuiEventUpdate(const SDL_Event& windowEvent) {
	switch (windowEvent.type) {
		case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			break;
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			sdl::Window::quit();
			break;
		case SDL_EVENT_QUIT:
			sdl::Window::quit();
			break;
		case SDL_EVENT_KEY_DOWN:
			switch (windowEvent.key.key) {
				case SDLK_ESCAPE:
					sdl::Window::quit();
					break;
			}
			break;
	}
}

void TestImGuiWindow::imGuiUpdate(const sdl::DeltaTime& deltaTime) {
	ImGui::MainWindow("Main", [&]() {
		ImGui::Button("Hello", {100, 100});
		ImGui::Button("Hello2", {50, 50});
	});
}

void TestImGuiWindow::imGuiPreUpdate(const sdl::DeltaTime& deltaTime) {
	
}

void TestImGuiWindow::initPreLoop() {
	ImGuiWindow::initPreLoop();
}
