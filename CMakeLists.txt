cmake_minimum_required(VERSION 3.31)
project(CppSdl3
	DESCRIPTION
		"A C++ wrapper around SDL3 (Simple Direct Media Layer) and SDL_gpu embedded with ImGui (Dear ImGui)"
	LANGUAGES
		CXX
	VERSION 0.12.0
)

add_library(CppSdl3 STATIC)
add_library(CppSdl3::CppSdl3 ALIAS CppSdl3)

include(GNUInstallDirs)
add_subdirectory(ImGui)

set(CPPSDL3_HEADERS
	src/sdl/batch.h
	src/sdl/color.h
	src/sdl/gamecontroller.h
	src/sdl/glm.h
	src/sdl/gpu.h
	src/sdl/gpuutil.h
	src/sdl/imageatlas.h
	src/sdl/sdlexception.h
	src/sdl/shader.h
	src/sdl/shader.vs.h
	src/sdl/shader.ps.h
	src/sdl/window.h
	src/sdl/util.h
)

set(CPPSDL3_SOURCES
	vcpkg.json
	CMakePresets.json
	cppsdl3.natstepfilter
	cppsdl3.natvis

	src/sdl/color.cpp
	src/sdl/gamecontroller.cpp
	src/sdl/glm.cpp
	src/sdl/gpuutil.cpp
	src/sdl/imageatlas.cpp
	src/sdl/shader.cpp
	src/sdl/window.cpp
	src/sdl/util.cpp
)

target_sources(CppSdl3
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS src
		FILES ${CPPSDL3_HEADERS}
	PRIVATE
		${CPPSDL3_SOURCES}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPPSDL3_SOURCES} ${CPPSDL3_HEADERS}) 

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

		CPPSDL3_VERSION="${PROJECT_VERSION}"
		CPPSDL3_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
		CPPSDL3_VERSION_MINOR=${PROJECT_VERSION_MINOR}
		CPPSDL3_VERSION_PATCH=${PROJECT_VERSION_PATCH}
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

if (MSVC)
	message(STATUS "MSVC_TOOLSET_VERSION: ${MSVC_VERSION}")
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
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cppsdl3
	FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cppsdl3
)
install(FILES
	${CMAKE_CURRENT_SOURCE_DIR}/cppsdl3.natstepfilter
	${CMAKE_CURRENT_SOURCE_DIR}/cppsdl3.natvis
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cppsdl3
)

include(CMakePackageConfigHelpers)

# Export targets
install(
	EXPORT CppSdl3Targets
	FILE CppSdl3Targets.cmake
	NAMESPACE CppSdl3::
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/CppSdl3
)

# Install PDB files for debugging support on Windows
if (MSVC)
	install(
		FILES $<TARGET_FILE_DIR:CppSdl3>/CppSdl3.pdb
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		CONFIGURATIONS Debug RelWithDebInfo
		OPTIONAL
	)
endif()

# Config file
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/CppSdl3Config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/CppSdl3Config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/CppSdl3
)

# Generate the version file
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/CppSdl3ConfigVersion.cmake
	COMPATIBILITY AnyNewerVersion
)

install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/CppSdl3Config.cmake
		${CMAKE_CURRENT_BINARY_DIR}/CppSdl3ConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/CppSdl3
)

install(
	FILES LICENSE.txt
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/CppSdl3
)
