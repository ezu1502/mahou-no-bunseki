cmake_minimum_required(VERSION 3.20)

project(bunseki LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYBIND11_FINDPYTHON ON)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

add_library(bunseki STATIC
    src/sample_reader.cpp
    src/analysis.cpp
    src/transform.cpp
    src/cbunseki.cpp
    src/buntools.cpp
    external/miniaudio.cpp
)

target_include_directories(bunseki
    PUBLIC include
    PRIVATE src external
)

add_executable(buntest
    src/main.cpp
)

set_target_properties(buntest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(buntest PRIVATE bunseki)

pybind11_add_module(pybunseki
    src/bindings.cpp
)

target_link_libraries(pybunseki PRIVATE bunseki)
