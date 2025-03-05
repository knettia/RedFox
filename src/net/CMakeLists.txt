# compile
add_library(redfox-net SHARED "connection.cpp" "socket.cpp")

# include
target_include_directories(redfox-net PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
	target_include_directories(redfox-net PRIVATE /usr/local/include)
endif()

# link
target_link_libraries(redfox-net PRIVATE redfox-core)