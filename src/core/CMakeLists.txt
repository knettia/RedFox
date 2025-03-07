# compile
set(TYPES_SOURCES "types/library.cpp")
set(CORE_SOURCES "string.cpp" "system.cpp" "monitor.cpp")

add_library(redfox-core SHARED ${CORE_SOURCES} ${TYPES_SOURCES} "utils/ignoramus.cpp")

# include
target_include_directories(redfox-core PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
	target_include_directories(redfox-core PRIVATE /usr/local/include)
elseif(WIN32)
	# TODO: instead of linking, dynamically load the DbgHelp library atc runtime using RF::library_m
	target_link_libraries(redfox-core PRIVATE "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\DbgHelp.Lib")
endif()	