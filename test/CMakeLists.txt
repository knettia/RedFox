set(TEST_SOURCES "monitor.cpp")

foreach(TEST_FILE ${TEST_SOURCES})
	get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
	add_executable(${TEST_NAME} ${TEST_FILE})
	target_link_libraries(${TEST_NAME} PRIVATE redfox-core)
	target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endforeach()

add_library(square SHARED "libsquare/libsquare.c")