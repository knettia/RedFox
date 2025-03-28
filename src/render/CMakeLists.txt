# compile
# -- INTERFACE:
set(VIDEO_MODE_SOURCES)
set(DELEGATE_SOURCES)
set(WINDOW_SOURCES)

if(UNIX AND NOT APPLE)
	# TODO: set sources for build
elseif(APPLE)
	set(VIDEO_MODE_SOURCES "interface/video_mode/cocoa_video_mode.mm")
	set(DELEGATE_SOURCES "interface/delegate/cocoa_delegate.mm")
	set(WINDOW_SOURCES "interface/window/cocoa_window.mm")
elseif(WIN32)
	set(DELEGATE_SOURCES "interface/delegate/win32_delegate.cpp")
	set(WINDOW_SOURCES "interface/window/win32_window.cpp")
endif()
set(INTERFACE_SOURCES ${VIDEO_MODE_SOURCES} "interface/window.cpp" ${WINDOW_SOURCES} "interface/delegate.cpp" ${DELEGATE_SOURCES})

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

if(APPLE)

	target_link_libraries(redfox-render PRIVATE "-framework Cocoa")
	target_link_libraries(redfox-render PRIVATE "-framework Carbon")
	
	target_link_libraries(redfox-render PRIVATE "-framework Metal")
	target_link_libraries(redfox-render PRIVATE "-framework QuartzCore")
endif()