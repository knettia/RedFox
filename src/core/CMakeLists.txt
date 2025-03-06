# compile
set(TYPES_SOURCES "types/library.cpp")
set(CORE_SOURCES "string.cpp" "system.cpp" "monitor.cpp")

add_library(redfox-core SHARED ${CORE_SOURCES} ${TYPES_SOURCES} "utils/ignoramus.cpp")

# include
target_include_directories(redfox-core PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
	target_include_directories(redfox-core PRIVATE /usr/local/include)
endif()