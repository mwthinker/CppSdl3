cmake_minimum_required(VERSION 3.24)
project(CppSdl3
	DESCRIPTION
		"A C++ Wrapper of SDL3"
	LANGUAGES
		CXX
)

set(SOURCES
	src/sdl/color.cpp
	src/sdl/color.h
	src/sdl/font.cpp
	src/sdl/font.h
	src/sdl/gamecontroller.cpp
	src/sdl/gamecontroller.h
	src/sdl/imguiauxiliary.cpp
	src/sdl/imguiauxiliary.h
	src/sdl/imguiwindow.h
	src/sdl/imguiwindow.cpp
	src/sdl/initsdl.cpp
	src/sdl/initsdl.h
	src/sdl/rect.h
	src/sdl/surface.cpp
	src/sdl/surface.h
	src/sdl/texture.cpp
	src/sdl/texture.h
	src/sdl/window.cpp
	src/sdl/window.h
	vcpkg.json
	CMakePresets.json
	CppSdl3.natstepfilter
)
add_library(CppSdl3 ${SOURCES})
add_library(CppSdl3::CppSdl3 ALIAS CppSdl3)
add_subdirectory(ImGui)

source_group(TREE
	${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		${SOURCES}
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(
	ImGui

	PROPERTIES FOLDER
		ExternalDependencies
)


if (MSVC)
	target_compile_options(CppSdl3
		PRIVATE
			/W3 /WX /permissive-
	)
else()
	target_compile_options(CppSdl3
		PRIVATE
			-Wall -Wextra -Wnon-virtual-dtor -pedantic -Wcast-align -Woverloaded-virtual -Wno-unused-parameter
	)
endif()

target_compile_definitions(CppSdl3
	PUBLIC
		NOMINMAX
)

target_include_directories(CppSdl3
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(CppSdl3
	PUBLIC
		ImGui
		spdlog::spdlog_header_only
)

set_target_properties(CppSdl3
	PROPERTIES
		CXX_STANDARD 23
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

message(STATUS "CppSdl3_Example is available to add: -DCppSdl3_Example=1")
option(CppSdl3_Example "Add CppSdl3_Example project." OFF)
if (CppSdl3_Example)
	add_subdirectory(CppSdl3_Example)
endif ()

message(STATUS "MSVC_TOOLSET_VERSION: ${MSVC_VERSION}")

option(CppSdl3_Test "Add CppSdl3_Test to project." OFF)
if (CppSdl3_Test)
	add_subdirectory(CppSdl3_Test)
endif ()
