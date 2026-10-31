cmake_minimum_required(VERSION 3.20)

project(bunseki LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYBIND11_FINDPYTHON ON)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

add_library(bunseki STATIC
    src/cpp/sample_reader.cpp
    src/cpp/analysis.cpp
    src/cpp/transform.cpp
    src/cpp/cbunseki.cpp
    src/cpp/buntools.cpp
    external/miniaudio.cpp
)

target_include_directories(bunseki
    PUBLIC include
    PRIVATE src/cpp external
)

add_executable(buntest
    src/cpp/main.cpp
)

set_target_properties(buntest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(buntest PRIVATE bunseki)

pybind11_add_module(_pybunseki
    src/cpp/bindings.cpp
)

set_target_properties(_pybunseki PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/python/bunseki"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/python/bunseki"

    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/src/python/bunseki"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/src/python/bunseki"

    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/src/python/bunseki"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/src/python/bunseki"
)

target_link_libraries(_pybunseki PRIVATE bunseki)

install(TARGETS _pybunseki
    LIBRARY DESTINATION bunseki
    RUNTIME DESTINATION bunseki
)