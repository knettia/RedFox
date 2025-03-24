# compile
set(CORE_SOURCES "exception.cpp" "string.cpp" "file.cpp" "system.cpp" "monitor.cpp" "library.cpp")

# append dialogues
if(UNIX AND NOT APPLE)
	set(CORE_SOURCES ${CORE_SOURCES} "monitor/gtk_dialogue.cpp")
elseif(APPLE)
	set(CORE_SOURCES ${CORE_SOURCES} "monitor/cocoa_dialogue.mm")
elseif(WIN32)
	set(CORE_SOURCES ${CORE_SOURCES} "monitor/win32_dialogue.cpp")
endif()

add_library(redfox-core SHARED ${CORE_SOURCES})

# include
target_include_directories(redfox-core PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
	target_include_directories(redfox-core PRIVATE /usr/local/include)
endif()

if(UNIX AND NOT APPLE)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK REQUIRED gtk+-3.0)

	target_compile_options(redfox-core PRIVATE ${GTK_CFLAGS_OTHER})
	target_include_directories(redfox-core PRIVATE ${GTK_INCLUDE_DIRS})
elseif(APPLE)
	target_link_libraries(redfox-core PRIVATE "-framework Cocoa")
elseif(WIN32)
	# TODO: instead of linking, dynamically load the DbgHelp library atc runtime using RF::library_m
	target_link_libraries(redfox-core PRIVATE "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\DbgHelp.Lib")
endif()