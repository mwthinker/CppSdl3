project(ImGui
	DESCRIPTION
		"ImGui branch: https://github.com/ocornut/imgui"
	LANGUAGES
		CXX
	# > VERSION 1.91.94
)

include(FetchContent)
FetchContent_Declare(RemoteImGui
	GIT_REPOSITORY
		https://github.com/ocornut/imgui.git
	GIT_TAG
		0a222a3e2adbda761ae8f3aeffd9254b976693e9
)
FetchContent_MakeAvailable(RemoteImGui)
FetchContent_GetProperties(RemoteImGui
	SOURCE_DIR
		RemoteImGui_SOURCE_DIR
)

set(IMGUI_USER_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/src/imconfig.h)

add_library(ImGui
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
	${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natvis
	${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natstepfilter
	${IMGUI_USER_CONFIG}
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_user.h
)

target_include_directories(ImGui
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
		$<BUILD_INTERFACE:${RemoteImGui_SOURCE_DIR}>
		$<BUILD_INTERFACE:${RemoteImGui_SOURCE_DIR}/backends>
		$<INSTALL_INTERFACE:include/CppSdl2/Imgui>
)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(SDL3_image CONFIG REQUIRED)

#$<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

# Compile error in GCC 2023-09-13
# find_package(unofficial-lunasvg CONFIG REQUIRED)
# Link to unofficial::lunasvg::lunasvg
# vcpkg "lunasvg"
# IMGUI_ENABLE_FREETYPE_LUNASVG

target_link_libraries(ImGui
	PUBLIC
		SDL3::SDL3
		#SDL3::SDL3_image
		$<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
		$<IF:$<TARGET_EXISTS:SDL3_ttf::SDL3_ttf>,SDL3_ttf::SDL3_ttf,SDL3_ttf::SDL3_ttf-static>
		#SDL3::SDL3_ttf
		freetype
		fmt::fmt
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
)

set_target_properties(ImGui
	PROPERTIES
		CXX_STANDARD 23
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

# -------------------------------------------------------------------------
install(TARGETS ImGui
	EXPORT CppSdl2Targets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

install(FILES
	${RemoteImGui_SOURCE_DIR}/misc/freetype/imgui_freetype.h
	DESTINATION include/CppSdl2/Imgui/misc/freetype
)
install(FILES
	${RemoteImGui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
	DESTINATION include/CppSdl2/Imgui/misc/cpp
)

install(FILES
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
	${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${RemoteImGui_SOURCE_DIR}/imgui.h
	${RemoteImGui_SOURCE_DIR}/imgui_internal.h
	${RemoteImGui_SOURCE_DIR}/imstb_rectpack.h
	${RemoteImGui_SOURCE_DIR}/imstb_textedit.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/imconfig.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_user.h
	DESTINATION include/CppSdl2/Imgui
)

install(EXPORT CppSdl2Targets
	FILE CppSdl2Targets.cmake
	NAMESPACE CppSdl2::
	DESTINATION share/cmake/CppSdl2
)
