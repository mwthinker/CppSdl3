project(ImGui
	DESCRIPTION
		"ImGui branch: https://github.com/ocornut/imgui"
	LANGUAGES
		CXX
	VERSION 1.92.5
)

include(FetchContent)
FetchContent_Declare(RemoteImGui
	GIT_REPOSITORY
		https://github.com/ocornut/imgui.git
	GIT_TAG
		3912b3d9a9c1b3f17431aebafd86d2f40ee6e59c
)
FetchContent_MakeAvailable(RemoteImGui)
FetchContent_GetProperties(RemoteImGui
	SOURCE_DIR
		RemoteImGui_SOURCE_DIR
)

set(IMGUI_USER_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/src/imconfig.h)

add_library(ImGui STATIC)
add_library(CppSdl3::ImGui ALIAS ImGui)

target_sources(ImGui
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS
			${RemoteImGui_SOURCE_DIR}
			${CMAKE_CURRENT_SOURCE_DIR}/src
			
		FILES
			${RemoteImGui_SOURCE_DIR}/imgui.h
			${RemoteImGui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
			${RemoteImGui_SOURCE_DIR}/misc/freetype/imgui_freetype.h
			${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_user.h
			${IMGUI_USER_CONFIG}
			${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
			${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h

	PRIVATE
		${RemoteImGui_SOURCE_DIR}/imgui.cpp
		${RemoteImGui_SOURCE_DIR}/imgui_demo.cpp
		${RemoteImGui_SOURCE_DIR}/imgui_draw.cpp
		${RemoteImGui_SOURCE_DIR}/imgui_internal.h
		${RemoteImGui_SOURCE_DIR}/imgui_tables.cpp
		${RemoteImGui_SOURCE_DIR}/imgui_widgets.cpp
		${RemoteImGui_SOURCE_DIR}/imstb_rectpack.h
		${RemoteImGui_SOURCE_DIR}/imstb_textedit.h
		${RemoteImGui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
		${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
		${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
		
		${RemoteImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3_shaders.h
		${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natvis
		${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natstepfilter
		${RemoteImGui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
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

		IMGUI_VERSION="${PROJECT_VERSION}"
		IMGUI_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
		IMGUI_VERSION_MINOR=${PROJECT_VERSION_MINOR}
		IMGUI_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

set_target_properties(ImGui
	PROPERTIES
		CXX_STANDARD 23
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

install(TARGETS ImGui
	EXPORT CppSdl3Targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cppsdl3/imgui
)

# Install PDB files for debugging support on Windows
if (MSVC)
	install(
		FILES $<TARGET_FILE_DIR:ImGui>/ImGui.pdb
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		CONFIGURATIONS Debug RelWithDebInfo
		OPTIONAL
	)
endif()

install(FILES
	${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natvis
	${RemoteImGui_SOURCE_DIR}/misc/debuggers/imgui.natstepfilter
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cppsdl3/imgui
)
