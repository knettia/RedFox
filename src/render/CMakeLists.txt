# compile
# -- INTERFACE:
set(DELEGATE_SOURCES)
set(WINDOW_SOURCES)

if(UNIX AND NOT APPLE)
	set(DELEGATE_SOURCES "interface/delegate/x11_delegate.cpp")
	set(WINDOW_SOURCES "interface/window/x11_window.cpp")
elseif(APPLE)
	set(DELEGATE_SOURCES "interface/delegate/cocoa_delegate.mm")
	set(WINDOW_SOURCES "interface/window/cocoa_window.mm")
elseif(WIN32)
	set(DELEGATE_SOURCES "interface/delegate/win32_delegate.cpp")
	set(WINDOW_SOURCES "interface/window/win32_window.cpp")
endif()
set(INTERFACE_SOURCES "interface/window.cpp" ${WINDOW_SOURCES} "interface/delegate.cpp" ${DELEGATE_SOURCES})

add_library(redfox-render SHARED ${INTERFACE_SOURCES})

# include
target_include_directories(redfox-render PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
	target_include_directories(redfox-render PRIVATE /usr/local/include)
endif()

# find for link
find_package(Vulkan REQUIRED)

# link
target_link_libraries(redfox-render PRIVATE redfox-core)
target_link_libraries(redfox-render PRIVATE Vulkan::Vulkan)

if(UNIX AND NOT APPLE)
	find_package(X11 REQUIRED)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(XRANDR REQUIRED xrandr)

	target_link_libraries(redfox-render PRIVATE ${X11_LIBRARIES})
	target_include_directories(redfox-render PRIVATE ${XRANDR_INCLUDE_DIRS})
	target_link_libraries(redfox-render PRIVATE ${XRANDR_LIBRARIES})
elseif(APPLE)

	target_link_libraries(redfox-render PRIVATE "-framework Cocoa")
	target_link_libraries(redfox-render PRIVATE "-framework Carbon")
	
	target_link_libraries(redfox-render PRIVATE "-framework Metal")
	target_link_libraries(redfox-render PRIVATE "-framework QuartzCore")
endif()
