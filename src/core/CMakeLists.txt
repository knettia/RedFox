# compile
if(UNIX AND NOT APPLE)
	set(MONITOR_SOURCES "interface/monitor/x11_monitor.cpp")
elseif(APPLE)
	set(MONITOR_SOURCES "interface/monitor/cocoa_monitor.mm")
endif()

set(TYPES_SOURCES "types/library.cpp")

add_library(redfox-core SHARED ${TYPES_SOURCES} "utils/ignoramus.cpp" ${MONITOR_SOURCES})

# include
target_include_directories(redfox-core PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
	target_include_directories(redfox-core PRIVATE /usr/local/include)
endif()

# link
if(APPLE)
	target_link_libraries(redfox-core PRIVATE "-framework Cocoa")
	target_link_libraries(redfox-core PRIVATE "-framework CoreGraphics")
	target_link_libraries(redfox-core PRIVATE "-framework IOKit")
endif()