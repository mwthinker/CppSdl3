#include "window.h"
#include "exception.h"

#include <spdlog/spdlog.h>
#include <glbinding/glbinding.h>
#include <glbinding/FunctionCall.h>
#include <glbinding/AbstractFunction.h>
#include <glbinding/gl/enum.h>

#include <thread>
#include <chrono>
#include <sstream>
#include <stdexcept>

namespace sdl {

	Window::Window() {
		spdlog::info("[sdl::Window] Creating Window");

		// SDL_VERSION hardcoded number from SDL headers
		spdlog::info("[sdl::Window] Compiled SDL Version: {}.{}.{}", 
			SDL_VERSIONNUM_MAJOR(SDL_VERSION),
			SDL_VERSIONNUM_MINOR(SDL_VERSION),
			SDL_VERSIONNUM_MICRO(SDL_VERSION)
		);
		const int linked = SDL_GetVersion(); // reported by linked SDL library
		spdlog::info("[sdl::Window] Linked SDL Version: {}.{}.{}",
			SDL_VERSIONNUM_MAJOR(linked),
			SDL_VERSIONNUM_MINOR(linked),
			SDL_VERSIONNUM_MICRO(linked));
	}

	Window::~Window() {
		if (icon_) {
			SDL_DestroySurface(icon_);
		}
		if (window_) {
			if (gpuDevice_) {
				SDL_ReleaseWindowFromGPUDevice(gpuDevice_, window_);
				SDL_DestroyGPUDevice(gpuDevice_);
			}
			SDL_DestroyWindow(window_);
		}
		spdlog::debug("[sdl::Window] Destructed.");
	}

	void Window::startLoop() {
		if (window_) {
			spdlog::warn("[sdl::Window] Loop already running");
			return;
		}

		spdlog::info("[sdl::Window] Init loop");
		auto flags = 0;
		if (resizable_) {
			flags |= SDL_WINDOW_RESIZABLE;
		}
		if (alwaysOnTop_) {
			flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		}
		if (!bordered_) {
			flags |= SDL_WINDOW_BORDERLESS;
		}
		
		initOpenGl();

		window_ = SDL_CreateWindow(
			title_.c_str(),
			width_,	height_,
			flags
		);
		if (window_ == nullptr) {
			spdlog::error("[sdl::Window] SDL_CreateWindow failed: {}", SDL_GetError());
			throw std::exception{};
		} else {
			spdlog::info("[sdl::Window] Windows {} created: \n\t(x, y) = ({}, {}) \n\t(w, h) = ({}, {}) \n\tflags = {}", title_, x_ == SDL_WINDOWPOS_UNDEFINED? -1 : x_, y_ == SDL_WINDOWPOS_UNDEFINED ? -1 : y_, width_, height_, flags);
		}
		gpuDevice_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB, true, nullptr);
		if (gpuDevice_ == nullptr) {
			spdlog::error("[sdl::Window] SDL_CreateGPUDevice failed: {}", SDL_GetError());
			throw std::exception{};
		}

		if (!SDL_ClaimWindowForGPUDevice(gpuDevice_, window_)) {
			spdlog::error("[sdl::Window] SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
			throw std::exception{};
		}
		SDL_SetGPUSwapchainParameters(gpuDevice_, window_, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);
		
		if (minWidth_ > 0 && minHeight_ > 0) {
			SDL_SetWindowMinimumSize(window_, minWidth_, minHeight_);
		}
		if (maxWidth_ > 0 && maxHeight_ > 0) {
			SDL_SetWindowMaximumSize(window_, maxWidth_, maxHeight_);
		}
		setOpacity(opacity_);

		if (icon_) {
			spdlog::debug("[sdl::Window] Windows icon updated");
			SDL_SetWindowIcon(window_, icon_);
			SDL_DestroySurface(icon_);
			icon_ = nullptr;
		}
		quit_ = false;
		setHitTestCallback(onHitTest_);

		initPreLoop();
		runLoop();
		spdlog::info("[sdl::Window] Loop ended");
	}

	void Window::runLoop() {
		spdlog::info("[sdl::Window] Loop starting");
		auto time = Clock::now();
		while (!quit_) {
			SDL_Event eventSDL;
			while (SDL_PollEvent(&eventSDL)) {
				eventUpdate(eventSDL);
			}

			auto currentTime = Clock::now();
			auto delta = currentTime - time;
			time = currentTime;
			update(delta);

			if (sleepingTime_ > std::chrono::nanoseconds{0}) {
				std::this_thread::sleep_for(sleepingTime_);
			}

			//SDL_SetRenderDrawColorFloat(renderer_, clearColor_.red(),clearColor_.green(), clearColor_.blue(), clearColor_.alpha());
			//SDL_RenderClear(renderer_);
		}
	}

	void Window::setOpacity(float opacity) {
		if (window_) {
			SDL_SetWindowOpacity(window_, opacity);
		} else {
			opacity_ = opacity;
		}
	}

	float Window::getOpacity() const {
		if (window_) {
			return SDL_GetWindowOpacity(window_);
		}
		return opacity_;
	}

	void Window::setBordered(bool bordered) {
		if (window_) {
			SDL_SetWindowBordered(window_, bordered);
			spdlog::info("[sdl::Window] Window border: {}", bordered);
		} else {
			bordered_ = bordered;
		}
	}

	void Window::setPosition(int x, int y) {
		if (window_) {
			if (x < 0) {
				x = SDL_WINDOWPOS_UNDEFINED;
			}
			if (y < 0) {
				y = SDL_WINDOWPOS_UNDEFINED;
			}			
			SDL_SetWindowPosition(window_, x, y);
			spdlog::info("[sdl::Window] Reposition window: (x, y) = ({}, {})", x == SDL_WINDOWPOS_UNDEFINED ? -1 : x, y == SDL_WINDOWPOS_UNDEFINED ? -1 : y);
		} else {
			x_ = x;
			y_ = y;
		}
	}

	void Window::setAlwaysOnTop(bool always) {
		if (window_) {
			spdlog::warn("[sdl::Window] Not supported after window creation!");
		} else {
			alwaysOnTop_ = always;
		}
	}

	bool Window::isAlwaysOnTop() const {
		return alwaysOnTop_;
	}

	void Window::setResizeable(bool resizable) {
		if (window_) {
			SDL_SetWindowResizable(window_, resizable);
			spdlog::info("[sdl::Window] Window resizeable: {}", resizable);
		} else {
			resizable_ = resizable;
		}
	}

	void Window::setIcon(const std::string& icon) {
		if (icon_) {
			SDL_DestroySurface(icon_);
			icon_ = nullptr;
		}
		//icon_ = IMG_Load(icon.c_str());
		if (window_ && icon_) {
			SDL_SetWindowIcon(window_, icon_);
			SDL_DestroySurface(icon_);
			icon_ = nullptr;
		}
	}

	void Window::setTitle(const std::string& title) {
		if (window_) {
			spdlog::info("[sdl::Window] tile named to {}", title);
			SDL_SetWindowTitle(window_, title.c_str());
		} else {
			title_ = title;
		}
	}

	Size Window::getSize() const {
		if (window_) {
			Size size;
			SDL_GetWindowSize(window_, &size.width, &size.height);
			return size;
		}
		return {width_, height_};;
	}

	Size Window::getMinSize() const {
		if (window_) {
			Size size;
			SDL_GetWindowMinimumSize(window_, &size.width, &size.height);
			return size;
		}
		return {minWidth_, minHeight_};;
	}

	Size Window::getMaxSize() const {
		if (window_) {
			Size size;
			SDL_GetWindowMaximumSize(window_, &size.width, &size.height);
			return size;
		}
		return {maxWidth_, maxHeight_};;
	}

	Size Window::getDrawableSize() const {
		if (window_) {
			Size size;
			SDL_GetWindowSizeInPixels(window_, &size.width, &size.height);
			return size;
		}
		return {width_, height_};;
	}

	Position Window::getWindowPosition() const {
		if (window_) {
			Position position;
			SDL_GetWindowPosition(window_, &position.x, &position.y);
			return position;
		}
		return {x_, y_};
	}

	void Window::setSize(int width, int height) {
		if (window_) {
			if (width < 1 || height < 1) {
				spdlog::warn("[sdl::Window] Resizing: (w, h) = ({}, {})", width, height);
				return;
			}

			spdlog::info("[sdl::Window] Resizing: (w, h) = ({}, {})", width, height);
			SDL_SetWindowSize(window_, width, height);
		} else {
			width_ = width;
			height_ = height;
		}
	}

	void Window::setMinSize(int width, int height) {
		if (window_) {
			if (width < 1 || height < 1) {
				return;
			}

			spdlog::info("[sdl::Window] Setting min size: (w, h) = ({}, {})", width, height);
			SDL_SetWindowMinimumSize(window_, width, height);
		} else {
			minWidth_ = width;
			minHeight_ = height;
		}
	}

	void Window::setMaxSize(int width, int height) {
		if (window_) {
			assert(width > 0 && height > 0);

			spdlog::info("[sdl::Window] Setting max size: (w, h) = ({}, {})", width, height);
			SDL_SetWindowMaximumSize(window_, width, height);
		} else {
			maxWidth_ = width;
			maxHeight_ = height;
		}
	}

	int Window::getWidth() const noexcept {
		return getSize().width;
	}

	int Window::getHeight() const noexcept {
		return getSize().height;
	}

	void Window::setFullScreen(bool fullScreen) {
		if (window_) {
			spdlog::info("[sdl::Window] Fullscreen is activated: {}", fullScreen);
			SDL_SetWindowFullscreen(window_, fullScreen);
		} else {
			fullScreen_ = fullScreen;
		}
	}

	bool Window::isFullScreen() const {
		return fullScreen_;
	}

	SDL_HitTestResult Window::hitTestCallback(SDL_Window* sdlWindow, const SDL_Point* area, void* data) {
		return static_cast<Window*>(data)->onHitTest_(*area);
	}

	void Window::setHitTestCallback(HitTestCallback onHitTest) {
		onHitTest_ = std::move(onHitTest);
		if (onHitTest_) {
			if (!SDL_SetWindowHitTest(window_, hitTestCallback, this)) {
				spdlog::warn("[sdl::Window] Window hit test failed to be initiated: {}", SDL_GetError());
			}
		} else {
			SDL_SetWindowHitTest(window_, nullptr, this);
		}
	}

}
