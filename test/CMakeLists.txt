set(TEST_SOURCES "window.cpp")

foreach(TEST_FILE ${TEST_SOURCES})
	get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
	add_executable(${TEST_NAME} ${TEST_FILE})
	target_link_libraries(${TEST_NAME} PRIVATE redfox-core)
	target_link_libraries(${TEST_NAME} PRIVATE redfox-render)
	target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

	if(APPLE)
		target_link_libraries(redfox-core PRIVATE "-framework Cocoa")
		target_link_libraries(redfox-core PRIVATE "-framework Metal")
		target_link_libraries(redfox-core PRIVATE "-framework QuartzCore")
	endif()
endforeach()

add_library(square SHARED "libsquare/libsquare.c")