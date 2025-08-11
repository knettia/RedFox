set(TEST_SOURCES "delegate.cpp")

foreach(TEST_FILE ${TEST_SOURCES})
	get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
	add_executable(${TEST_NAME} ${TEST_FILE})
	target_link_libraries(${TEST_NAME} PRIVATE core)
	target_link_libraries(${TEST_NAME} PRIVATE render)
	target_link_libraries(${TEST_NAME} PRIVATE net)

	if(UNIX)
		target_include_directories(${TEST_NAME} PRIVATE /usr/local/include)
	endif()

	target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endforeach()
