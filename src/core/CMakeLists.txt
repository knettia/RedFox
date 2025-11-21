# compile
set(CORE_SOURCES "exception.cpp" "string.cpp" "file.cpp" "image.cpp" "system.cpp" "monitor.cpp" "library.cpp" "definitions.cpp")

# append dialogues
if(UNIX AND NOT APPLE)
	set(CORE_SOURCES ${CORE_SOURCES} "monitor/gtk_dialogue.cpp")
elseif(APPLE)
	set(CORE_SOURCES ${CORE_SOURCES} "monitor/cocoa_dialogue.mm")
elseif(WIN32)
	set(CORE_SOURCES ${CORE_SOURCES} "monitor/win32_dialogue.cpp")
endif()

add_library(core SHARED ${CORE_SOURCES})

# include
# target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(core PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
	$<INSTALL_INTERFACE:include>)

if(UNIX AND NOT APPLE)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK REQUIRED gtk+-3.0)

	target_compile_options(core PUBLIC ${GTK_CFLAGS_OTHER})
	target_include_directories(core PUBLIC ${GTK_INCLUDE_DIRS})
endif()

if(UNIX)
	target_include_directories(core PUBLIC /usr/local/include)
endif()

# link
if(KTX_SUPPORT)
	target_compile_definitions(core PUBLIC KTX_SUPPORT)

	find_package(Ktx REQUIRED)
	target_link_libraries(core PUBLIC KTX::ktx)
endif()

if(DDS_SUPPORT)
	target_compile_definitions(core PUBLIC DDS_SUPPORT)
endif()

find_package(OpenEXR REQUIRED)
include_directories(${OpenEXR_INCLUDE_DIRS})
target_link_libraries(core PUBLIC OpenEXR::OpenEXR)

if(APPLE)
	target_link_libraries(core PUBLIC "-framework Cocoa")
elseif(WIN32)
	# TODO: instead of linking, dynamically load the DbgHelp library atc runtime using RF::library_m
	target_link_libraries(core PUBLIC "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\DbgHelp.Lib")
endif()

set_target_properties(core PROPERTIES OUTPUT_NAME "redfox-core")
