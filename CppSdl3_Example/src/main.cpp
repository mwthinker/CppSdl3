#include "testwindow.h"

#include "types.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fmt/core.h>
#include <SDL3_image/SDL_image.h>

#include <cassert>
#include <sstream>

using namespace sdl::color;

void testPrintColors() {
	for (const auto& [name, color] : html::getHtmlColors()) {
		spdlog::info("{}: {}", name, color.toHexString());
	}
}

void testImGuiWindow() {
	auto red = sdl::makeSdlUnique<SDL_Surface, SDL_DestroySurface>(createSurface(200, 100, Red));
	auto green = sdl::makeSdlUnique<SDL_Surface, SDL_DestroySurface>(createSurface(100, 200, Green));
	auto blue = sdl::makeSdlUnique<SDL_Surface, SDL_DestroySurface>(createSurface(200, 200, Blue));
	auto white = sdl::makeSdlUnique<SDL_Surface, SDL_DestroySurface>(createSurface(30, 30, White));
	auto tetris = sdl::makeSdlUnique<SDL_Surface, SDL_DestroySurface>(IMG_Load("tetris.bmp"));
	auto cross = sdl::makeSdlUnique<SDL_Surface, SDL_DestroySurface>(IMG_Load("cross.png"));

	spdlog::info("[testLoadTextureAtlas2] Error {}", SDL_GetError());
	static auto htmlColors = html::getHtmlColors();
	static int index = -1;

	TestWindow w;
	int nbr = 0;
	auto func = [&]() {
		++nbr;
		switch (nbr) {
			case 1:
				w.addSurfaceToAtlas(tetris.get(), 1);
				break;
			case 2:
				w.addSurfaceToAtlas(cross.get(), 1);
				break;
			case 3:
				w.addSurfaceToAtlas(red.get(), 10);
				break;
			case 4:
				w.addSurfaceToAtlas(green.get(), 5);
				break;
			case 5:
				w.addSurfaceToAtlas(blue.get(), 50);
				break;
			default:
				if (++index < static_cast<int>(htmlColors.size())) {
					auto html = htmlColors[index];
					auto colorSurface = sdl::makeSdlUnique<SDL_Surface, SDL_DestroySurface>(createSurface(30, 30, html.color));
					w.addSurfaceToAtlas(colorSurface.get(), 1);
					spdlog::info("[testLoadTextureAtlas2] Color added {} = {} ", html.name, html.color.toHexString());
				} else {
					w.addSurfaceToAtlas(white.get(), 1);
					spdlog::info("[testLoadTextureAtlas2] Add white");
				}
				break;
		}
	};

	w.setSpaceFunction(func);
	w.startLoop();
}

void showHelp(const std::string& programName) {
	fmt::println("Usage: {}", programName);
	fmt::println("\t{} -1 ", programName);
	fmt::println("\t{} -2 ", programName);
	fmt::println("");
	fmt::println("Options:");
	fmt::println("\t-h --help                show this help");
	fmt::println("\t-1                       testPrintColors");
	fmt::println("\t-2                       testImGuiWindow");
}

void runAll() {
	testPrintColors();
	testImGuiWindow();
}

void initLogger() {
	try {
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::debug);
		console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
		file_sink->set_level(spdlog::level::trace);

		spdlog::sinks_init_list sink_list = {file_sink, console_sink};
		//spdlog::flush_every(std::chrono::seconds{5});

		spdlog::logger logger{"multi_sink_example", sink_list.begin(), sink_list.end()};
		logger.set_level(spdlog::level::debug);
		logger.warn("this should appear in both console and file");
		logger.info("this message should not appear in the console, only in the file");
		logger.error("error text");

		// or you can even set multi_sink logger as default logger
		spdlog::set_default_logger(std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink, file_sink})));
	} catch (const spdlog::spdlog_ex& ex) {
		fmt::println("Log initialization failed: {}", ex.what());
	}
}

int main(int argc, char** argv) {
	initLogger();

	if (argc >= 2) {
		std::string programName = *argv;
		std::string code{*(argv + 1)};
		if (code == "-h" || code == "--help") {
			showHelp(programName);
			return 0;
		} else if (code == "-1") {
			testPrintColors();
		} else if (code == "-2") {
			testImGuiWindow();
			return 0;
		} else {
			fmt::println("Incorrect argument {}", code);
		}
	} else {
		runAll();
	}

	return 0;
}
