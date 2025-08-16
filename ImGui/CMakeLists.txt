project(ImGui
	DESCRIPTION
		"ImGui branch: https://github.com/ocornut/imgui"
	LANGUAGES
		CXX
	VERSION 1.92.2
)

include(FetchContent)
FetchContent_Declare(RemoteImGui
	GIT_REPOSITORY
		https://github.com/ocornut/imgui.git
	GIT_TAG
		1f7f1f54af38b0350d8c0008b096a9af6de299c7
)
FetchContent_MakeAvailable(RemoteImGui)
FetchContent_GetProperties(RemoteImGui
	SOURCE_DIR
		RemoteImGui_SOURCE_DIR
)

set(IMGUI_USER_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/src/imconfig.h)

add_library(ImGui STATIC
	${RemoteImGui_SOURCE_DIR}/imgui.cpp
	${RemoteImGui_SOURCE_DIR}/imgui.h
	${RemoteImGui_SOURCE_DIR}/imgui_demo.cpp
	${RemoteImGui_SOURCE_DIR}/imgui_draw.cpp
	${RemoteImGui_SOURCE_DIR}/imgui_internal.h
	${RemoteImGui_SOURCE_DIR}/imgui_tables.cpp
	${RemoteImGui_SOURCE_DIR}/imgui_widgets.cpp
	${RemoteImGui_SOURCE_DIR}/imstb_rectpack.h
	${RemoteImGui_SOURCE_DIR}/imstb_textedit.h
	${RemoteImGui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
	${RemoteImGui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
	${RemoteImGui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
	${RemoteImGui_SOURCE_DIR}/misc/freetype/imgui_freetype.h
		
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3_shaders.h
	${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natvis
	${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natstepfilter
	${IMGUI_USER_CONFIG}
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_user.h
)
add_library(CppSdl3::ImGui ALIAS ImGui)

target_include_directories(ImGui
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
		$<BUILD_INTERFACE:${RemoteImGui_SOURCE_DIR}>
		$<BUILD_INTERFACE:${RemoteImGui_SOURCE_DIR}/backends>
		$<INSTALL_INTERFACE:include/cppsdl3/imgui>
)

find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3)
find_package(glm CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)

target_link_libraries(ImGui
	PUBLIC
		SDL3::SDL3
		$<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
		SDL3_ttf::SDL3_ttf
		freetype
		glm::glm
)

target_compile_definitions(ImGui
	PUBLIC
		IMGUI_DISABLE_OBSOLETE_KEYIO
		IMGUI_DISABLE_OBSOLETE_FUNCTIONS
		IMGUI_INCLUDE_IMGUI_USER_H
		IMGUI_ENABLE_FREETYPE
		IMGUI_USER_CONFIG=\"${IMGUI_USER_CONFIG}\"
		IMGUI_DEFINE_MATH_OPERATORS
		GLM_ENABLE_EXPERIMENTAL
		GLM_FORCE_DEFAULT_ALIGNED_GENTYPES # To be compatible with UBO that assumes std140 layout
)

set_target_properties(ImGui
	PROPERTIES
		CXX_STANDARD 23
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

# -------------------------------------------------------------------------
install(TARGETS ImGui
	EXPORT CppSdl3Targets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

# Install specific files from the misc directory
install(FILES
	${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natvis
	${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natstepfilter
	DESTINATION include/cppsdl3/imgui/misc/debuggers
)
install(FILES
	${RemoteImGui_SOURCE_DIR}/misc/freetype/imgui_freetype.h
	DESTINATION include/cppsdl3/imgui/misc/freetype
)
install(FILES
	${RemoteImGui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
	DESTINATION include/cppsdl3/imgui/misc/cpp
)

# Install specific files from the root of RemoteImGui_SOURCE_DIR
install(FILES
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3_shaders.h
	${RemoteImGui_SOURCE_DIR}/imgui.h
	${RemoteImGui_SOURCE_DIR}/imgui_internal.h
	${RemoteImGui_SOURCE_DIR}/imstb_rectpack.h
	${RemoteImGui_SOURCE_DIR}/imstb_textedit.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/imconfig.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_user.h
	DESTINATION include/cppsdl3/imgui
)

install(EXPORT CppSdl3Targets
	FILE CppSdl3Targets.cmake
	NAMESPACE CppSdl3::
	DESTINATION share/cmake/cppsdl3
)
