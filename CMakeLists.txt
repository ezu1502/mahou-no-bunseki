cmake_minimum_required(VERSION 3.21)

project(bunseki LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYBIND11_FINDPYTHON ON)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

find_package(taglib CONFIG REQUIRED
    PATHS "C:/dev/vcpkg/installed/x64-windows-static-md/share/taglib"
    NO_DEFAULT_PATH
)

add_library(bunseki STATIC
    src/cpp/sample_reader.cpp
    src/cpp/analysis.cpp
    src/cpp/transform.cpp
    src/cpp/cbunseki.cpp
    src/cpp/buntools.cpp
    src/cpp/metadata.cpp
    external/miniaudio.cpp
)

target_link_libraries(bunseki PUBLIC
    TagLib::tag
)
target_include_directories(bunseki PUBLIC
    "C:/dev/vcpkg/installed/x64-windows-static-md/include"
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

target_link_libraries(_pybunseki PRIVATE bunseki)

add_custom_command(TARGET _pybunseki POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_RUNTIME_DLLS:_pybunseki>"
        "$<TARGET_FILE_DIR:_pybunseki>"
    COMMAND_EXPAND_LISTS
)

# add_custom_command(TARGET _pybunseki POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "C:/dev/vcpkg/installed/x64-windows-static-md/bin/libz.dll"
#         "$<TARGET_FILE_DIR:_pybunseki>"
# )

install(TARGETS _pybunseki
    LIBRARY DESTINATION bunseki
    RUNTIME DESTINATION bunseki
    # RUNTIME_DEPENDENCY_SET pybunseki_runtime
)

# Testing

include(CTest)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests/cpp)
endif()