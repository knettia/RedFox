cmake_minimum_required(VERSION 3.25.1)

set(PROJECT_NAME "RedFox Engine")
project(${PROJECT_NAME} LANGUAGES C CXX OBJCXX)

if(WIN32)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # for windows
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(KTX_SUPPORT "Compile RF::load_image with KTX support" ON)

add_subdirectory(src) # libredfox-core, libredfox-render, libredfox-network
add_subdirectory(test) # compile test sources