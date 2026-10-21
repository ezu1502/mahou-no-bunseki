cmake_minimum_required(VERSION 3.20)

project(bunseki LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(buntest
    src/main.cpp
    src/sample_reader.cpp
    external/miniaudio.cpp
)

target_include_directories(buntest PRIVATE
    src
    external
)

set_target_properties(buntest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
)