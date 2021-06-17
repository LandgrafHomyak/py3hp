cmake_minimum_required(VERSION 3.16)
project(pyhp C)

set(CMAKE_C_STANDARD 11)

set(PYTHON_INCLUDE C:/Python/Include/cpython-3.0)
set(PYHP_INCLUDE ./Include)

add_executable(
        PyHP.EXE

        ./api/main.c
        ./api/encoding.c
        ./api/parser.c
        ./api/compiler.c
        ./api/executor.c
        ./api/exports.def
)
set_target_properties(PyHP.EXE PROPERTIES OUTPUT_NAME pyhp)


include_directories(PyHP.EXE ${PYTHON_INCLUDE})
include_directories(PyHP.EXE ${PYHP_INCLUDE})

add_library(
        PyHP_API
        SHARED

        ./api/main.c
        ./api/encoding.c
        ./api/parser.c
        ./api/compiler.c
        ./api/executor.c
        ./api/exports.def
)
set_target_properties(PyHP_API PROPERTIES OUTPUT_NAME PyHP_API)

include_directories(PyHP_API ${PYTHON_INCLUDE})
include_directories(PyHP_API ${PYHP_INCLUDE})


add_library(
        PyHP_Core
        SHARED

        ./core/main.c
        ./core/parser.c
        ./core/compiler.c
        ./core/executor.c
        ./core/exports.def
)

add_dependencies(PyHP_Core PyHP_API)

set_target_properties(PyHP_Core PROPERTIES OUTPUT_NAME PyHP_Core)

include_directories(PyHP_Core ${PYTHON_INCLUDE})
include_directories(PyHP_Core ${PYHP_INCLUDE})

