# compile
set(TYPES_SOURCES "types/library.cpp")

add_library(redfox-core SHARED ${TYPES_SOURCES} "utils/ignoramus.cpp")

# include
target_include_directories(redfox-core PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
	target_include_directories(redfox-core PRIVATE /usr/local/include)
endif()