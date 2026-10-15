cmake_minimum_required(VERSION 3.20)
project(Bunseki LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Localiza Python e pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

#
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

find_package(pybind11 CONFIG REQUIRED)

# Código compartilhado da biblioteca
set(BUNSEKI_SOURCES
    bunseki/cpp/analysis.cpp
    bunseki/cpp/sample_reader.cpp
    bunseki/cpp/transform.cpp
)

# Executável independente pra testes
add_executable(main
    bunseki/cpp/main.cpp
    ${BUNSEKI_SOURCES}
)

target_include_directories(main PRIVATE
    bunseki/cpp
    external
)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
)


# Extensão para o Python
pybind11_add_module(_bunseki
    bunseki/cpp/bindings.cpp
    ${BUNSEKI_SOURCES}
)


target_include_directories(_bunseki PRIVATE
    bunseki/cpp
    external
)
# Configura onde o arquivo .pyd vai sair
set_target_properties(_bunseki PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bunseki/python/bunseki"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bunseki/python/bunseki"

    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bunseki/python/bunseki"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bunseki/python/bunseki"

    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bunseki/python/bunseki"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bunseki/python/bunseki"
)

install(TARGETS _bunseki DESTINATION bunseki)