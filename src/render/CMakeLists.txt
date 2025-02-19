# compile
set(WINDOW_SOURCES "interface/window.cpp")

if(UNIX AND NOT APPLE)
	# set(DELEGATE_SOURCES ${DELEGATE_SOURCES} "interface/monitor/x11_monitor.cpp")
	# set(DELEGATE_SOURCES ${DELEGATE_SOURCES} "interface/monitor/wl_monitor.cpp")
elseif(APPLE)
	set(WINDOW_SOURCES ${WINDOW_SOURCES} "interface/window/cocoa_window.mm")
elseif(WIN32)	
	# set(DELEGATE_SOURCES ${DELEGATE_SOURCES} "interface/monitor/win32_monitor.mm")
endif()

add_library(redfox-render SHARED ${WINDOW_SOURCES})

# include
target_include_directories(redfox-render PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
	target_include_directories(redfox-render PRIVATE /usr/local/include)
endif()

# link
if(APPLE)
	target_link_libraries(redfox-render PRIVATE "-framework Cocoa")
	target_link_libraries(redfox-render PRIVATE "-framework Metal")
	target_link_libraries(redfox-render PRIVATE "-framework QuartzCore")
endif()