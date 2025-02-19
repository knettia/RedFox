cmake_minimum_required(VERSION 3.25.1)


set(PROJECT_NAME "RedFox Engine")
project(${PROJECT_NAME} LANGUAGES C OBJC CXX OBJCXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src) # libredfox-core, libredfox-render, libredfox-network
add_subdirectory(test) # compile test sources