cmake_minimum_required(VERSION 3.14)
project(CppSdl3_Example
	DESCRIPTION
		"An example of using the CppSdl3 library"
	LANGUAGES
		CXX
)

set(SOURCES
	src/testimguiwindow.cpp
	src/testimguiwindow.h
	src/types.h
	src/main.cpp
	src/testwindow.cpp
	src/testwindow.h
)

add_executable(CppSdl3_Example ${SOURCES})

source_group(TREE
	${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		${SOURCES}
)

if (MSVC)
	target_compile_options(CppSdl3_Example
		PRIVATE
			/W3 /WX /permissive-
	)
else()
	target_compile_options(CppSdl3_Example
		PRIVATE
			-Wall -Wextra -Wnon-virtual-dtor -pedantic -Wcast-align -Woverloaded-virtual -Wno-unused-parameter
	)
endif()
	
target_link_libraries(CppSdl3_Example
	PRIVATE
		CppSdl3
)

set_target_properties(CppSdl3_Example
	PROPERTIES
		CXX_STANDARD 23
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)
	
file(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
