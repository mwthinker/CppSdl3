cmake_minimum_required(VERSION 3.31)
project(CppSdl3
	DESCRIPTION
		"A C++ Wrapper of SDL3"
	LANGUAGES
		CXX
	VERSION 0.5.0
)

set(SOURCES
	src/sdl/gpuutil.h
	src/sdl/gpuutil.cpp
	src/sdl/gpu.h
	
	src/sdl/color.cpp
	src/sdl/color.h
	src/sdl/gamecontroller.cpp
	src/sdl/gamecontroller.h
	src/sdl/glm.cpp
	src/sdl/glm.h
	src/sdl/imageatlas.cpp
	src/sdl/imageatlas.h
	src/sdl/sdlexception.h
	src/sdl/shader.cpp
	src/sdl/shader.h
	src/sdl/shader.vs.h
	src/sdl/shader.ps.h
	src/sdl/window.cpp
	src/sdl/window.h
	src/sdl/util.cpp
	src/sdl/util.h

	vcpkg.json
	CMakePresets.json
	cppsdl3.natstepfilter
	cppsdl3.natvis
)
add_library(CppSdl3 STATIC
	${SOURCES}
)
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
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
		$<INSTALL_INTERFACE:include/cppsdl3>
)

find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

target_link_libraries(CppSdl3
	PUBLIC
		CppSdl3::ImGui
		spdlog::spdlog_header_only
		fmt::fmt
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

# -------------------------------------------------------------------------
# Install
install(TARGETS CppSdl3
	EXPORT CppSdl3Targets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include/cppsdl3
)

install(DIRECTORY src/sdl/
	DESTINATION include/cppsdl3/sdl
	FILES_MATCHING PATTERN "*.h"
)
install(FILES
	${CMAKE_CURRENT_SOURCE_DIR}/cppsdl3.natstepfilter
	DESTINATION include/cppsdl3
)

include(CMakePackageConfigHelpers)

# Export the package configuration
install(EXPORT CppSdl3Targets
	FILE CppSdl3Targets.cmake
	NAMESPACE CppSdl3::
	DESTINATION share/cmake/cppsdl3
)

# Generate the package configuration file
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/CppSdl3Config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/CppSdl3Config.cmake
	INSTALL_DESTINATION share/cmake/cppsdl3
)

# Install the generated package configuration file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CppSdl3Config.cmake
	DESTINATION share/cmake/cppsdl3
)

# Install the package configuration file
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/CppSdl3ConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CppSdl3ConfigVersion.cmake
	DESTINATION share/cmake/cppsdl3
)

install(FILES LICENSE.txt DESTINATION share/doc/cppsdl3)
