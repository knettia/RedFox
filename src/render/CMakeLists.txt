# compile
# -- INTERFACE:
set(DELEGATE_SOURCES)
set(WINDOW_SOURCES)

if(UNIX AND NOT APPLE)
	set(DELEGATE_SOURCES "interface/delegate/x11_delegate.cpp")
	set(WINDOW_SOURCES "interface/window/x11_window.cpp")
	
	set(INTERFACE_SOURCES "interface/dyfuncs/x11_dyfuncs.cpp")
elseif(APPLE)
	set(DELEGATE_SOURCES "interface/delegate/cocoa_delegate.mm")
	set(WINDOW_SOURCES "interface/window/cocoa_window.mm")
elseif(WIN32)
	set(DELEGATE_SOURCES "interface/delegate/win32_delegate.cpp")
	set(WINDOW_SOURCES "interface/window/win32_window.cpp")
endif()

set(INTERFACE_SOURCES ${INTERFACE_SOURCES} "interface/window.cpp" ${WINDOW_SOURCES} "interface/delegate.cpp" ${DELEGATE_SOURCES})


set(VULKAN_SOURCES
    "vulkan/instance.cpp"
    "vulkan/device.cpp"
    "vulkan/debugger.cpp"
    "vulkan/buffer.cpp"
    "vulkan/image.cpp"
    "vulkan/framebuffer.cpp"
    "vulkan/render_pass.cpp"
    "vulkan/descriptor_pool.cpp"
    "vulkan/descriptor_set.cpp"
    "vulkan/pipeline.cpp"
    "vulkan/swapchain.cpp")

add_library(render SHARED ${INTERFACE_SOURCES} ${VULKAN_SOURCES})

# include
target_include_directories(render PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
	$<INSTALL_INTERFACE:include>)

if(UNIX)
	target_include_directories(render PUBLIC /usr/local/include)
endif()

# find for link
find_package(Vulkan REQUIRED)

# link
target_link_libraries(render PUBLIC core)
target_link_libraries(render PUBLIC Vulkan::Vulkan)

if(UNIX AND NOT APPLE)
	find_package(X11 REQUIRED)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(XRANDR REQUIRED xrandr)

	target_include_directories(render PUBLIC ${X11_INCLUDE_DIRS})
	target_include_directories(render PUBLIC ${XRANDR_INCLUDE_DIRS})
elseif(APPLE)
	target_link_libraries(render PUBLIC "-framework Cocoa")
	target_link_libraries(render PUBLIC "-framework Carbon")
	
	target_link_libraries(render PUBLIC "-framework Metal")
	target_link_libraries(render PUBLIC "-framework QuartzCore")
endif()

set_target_properties(render PROPERTIES OUTPUT_NAME "redfox-render")
