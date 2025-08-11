# compile
add_library(net SHARED "connection.cpp" "socket.cpp")

# include
target_include_directories(net PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
	$<INSTALL_INTERFACE:include>)

if(UNIX)
	target_include_directories(net PUBLIC /usr/local/include)
endif()

# link
target_link_libraries(net PUBLIC core)

set_target_properties(net PROPERTIES OUTPUT_NAME "redfox-net")
