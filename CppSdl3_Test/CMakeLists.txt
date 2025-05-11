project(CppSdl3_Test
	DESCRIPTION
		"Test the project CppSdl3 using GoogleTest"
	LANGUAGES
		CXX
)

find_package(GTest CONFIG REQUIRED)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4275") # non - DLL-interface class 'class_1' used as base for DLL-interface class 'class_2'
endif ()

add_executable(CppSdl3_Test
	src/tests.cpp
)

target_link_libraries(CppSdl3_Test
	PRIVATE
		CppSdl3
		GTest::gtest GTest::gtest_main
)

set_target_properties(CppSdl3_Test
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

if (MSVC)
	target_compile_options(CppSdl3
		PRIVATE
			"/permissive-"
	)
endif ()

include(GoogleTest)
gtest_discover_tests(CppSdl3_Test)
