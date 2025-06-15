#ifndef CPPSDL3_SDL_WINDOW_H
#define CPPSDL3_SDL_WINDOW_H

#include "color.h"
#include "gpu/gpucontext.h"
#include "util.h"

#include <SDL3/SDL.h>

#include <chrono>
#include <string>
#include <utility>
#include <functional>
#include <memory>

namespace sdl {

	// Create a window which handle all user input. The graphic is rendered using SDL_gpu.
	class Window {
	public:
		using HitTestCallback = std::function<SDL_HitTestResult(const SDL_Point&)>;

		Window();

		virtual ~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		// Start the loop which handle all inputs and graphics in the windows. It will not
		// return until the loop is ended. Is closed when the window is closed, i.e. a
		// call to the function quit().
		void startLoop();

		// Make the program to quit as soon as the current frame is finished.
		// I.e. the loop in startLoop() will be made to stop and startLoop() will return.
		void quit() noexcept;

		// The id for the windows. Is the same as calling SDL_GetWindowID.
		Uint32 getId() const;

		// Return the window pointer. Use with care.
		SDL_Window* getSdlWindow() const noexcept;

		//  Return the renderer pointer. Use with care.
		SDL_GPUDevice* getSdlGpuDevice() const noexcept;

		void setPosition(int x, int y);

		void setSize(int width, int height);
		void setIcon(const std::string& icon);

		void setTitle(const std::string& title);

		void setClearColor(Color color) noexcept;

		Color getClearColor() const noexcept {
			return clearColor_;
		}
		
		void setLoopSleepingTime(const std::chrono::nanoseconds& delay) noexcept;
		
		std::chrono::nanoseconds getLoopSleepingTime() const noexcept;

		void setHitTestCallback(HitTestCallback onHitTest);

		bool isHitTestCallbackSet() const {
			return static_cast<bool>(onHitTest_);
		}

		bool isShowDemoWindow() const;
		void setShowDemoWindow(bool show);

		bool isShowColorWindow() const;
		void setShowColorWindow(bool show);

	protected:
		virtual void preLoop() {}
		virtual void postLoop() {}
		
		// Is called each loop cycle until the event queue is empty.
		virtual void processEvent(const SDL_Event& windowEvent) {}
		virtual void renderImGui(const DeltaTime& deltaTime) {};
		
		// Override to add custom SDL_gpu rendering that is drawn before the ImGui rendering.
		// Any override should clear the swapchainTexture.
		// Default implementation clears the screen with the clear color.
		virtual void renderFrame(const DeltaTime& deltaTime, SDL_GPUTexture* swapchainTexture, SDL_GPUCommandBuffer* commandBuffer);

		SDL_Window* window_ = nullptr;
		Color clearColor_;
		std::chrono::nanoseconds sleepingTime_{};
		bool quit_ = false;
		gpu::GpuContext gpuContext_;
	private:
		// Is called each frame.
		void renderFrame(const DeltaTime& deltaTime);

		static SDL_HitTestResult hitTestCallback(SDL_Window* sdlWindow, const SDL_Point* area, void* data);

		static constexpr int DefaultWidth = 800;
		static constexpr int DefaultHeight = 800;

		void runLoop();

		HitTestCallback onHitTest_;
		SDL_Surface* icon_ = nullptr;
		
		std::string title_;
		int width_ = DefaultWidth;
		int height_ = DefaultHeight;
		int x_ = SDL_WINDOWPOS_UNDEFINED;
		int y_ = SDL_WINDOWPOS_UNDEFINED;
		
		bool showDemoWindow_ = false;
		bool showColorWindow_ = false;
		SDL_WindowFlags flags_ = SDL_WINDOW_RESIZABLE;
	};

	inline void Window::quit() noexcept {
		quit_ = true;
	}

	inline Uint32 Window::getId() const {
		return SDL_GetWindowID(window_);
	}

	inline SDL_Window* Window::getSdlWindow() const noexcept {
		return window_;
	}

	inline SDL_GPUDevice* Window::getSdlGpuDevice() const noexcept {
		return gpuContext_.getGpuDevice();
	}

	inline void Window::setClearColor(Color color) noexcept {
		clearColor_ = color;
	}

	inline void Window::setLoopSleepingTime(const std::chrono::nanoseconds& delay) noexcept {
		sleepingTime_ = delay;
	}

	inline std::chrono::nanoseconds Window::getLoopSleepingTime() const noexcept {
		return sleepingTime_;
	}

	inline bool Window::isShowDemoWindow() const {
		return showDemoWindow_;
	}

	inline void Window::setShowDemoWindow(bool show) {
		showDemoWindow_ = show;
	}

	inline bool Window::isShowColorWindow() const {
		return showColorWindow_;
	}

	inline void Window::setShowColorWindow(bool show) {
		showColorWindow_ = show;
	}

}

#endif
