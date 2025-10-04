cmake_minimum_required(VERSION 3.25.1)

set(PROJECT_NAME "RedFox")
set(PROJECT_VERSION "0.3.0")
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX OBJCXX)

if(WIN32)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # for windows
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(KTX_SUPPORT "Compile RF::load_image with KTX support" ON)
option(DDS_SUPPORT "Compile RF::load_image with DDS support" ON)

add_subdirectory(src) # redfox-core, redfox-render, redfox-net
add_subdirectory(test) # compile test sources

install(TARGETS
	core
	render
	net
	EXPORT RedFoxTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT RedFoxTargets
	NAMESPACE RF::
	FILE RedFoxTargets.cmake
	DESTINATION lib/cmake/RedFox)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/RedFoxConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/RedFoxConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/RedFoxConfig.cmake"
	INSTALL_DESTINATION lib/cmake/RedFox
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/RedFoxConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/RedFoxConfigVersion.cmake"
	DESTINATION lib/cmake/RedFox
)
