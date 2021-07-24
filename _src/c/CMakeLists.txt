cmake_minimum_required(VERSION 3.16)
project(pyhp C)

find_package(Python COMPONENTS Development)

include_directories(${Python_INCLUDE_DIRS})
link_directories(${Python_LIBRARY_DIRS})
link_libraries(${Python_LIBRARIES})
link_directories(${Python_RUNTIME_LIBRARY_DIRS})

set(CMAKE_C_STANDARD 11)

set(PYHP_INCLUDE ../../Include)

add_executable(
        PyHP.EXE

        ./api/main.c
        ../../src/encoding.c
        ../../src/parser.c
        ../../src/compiler.c
        ../../src/executor.c
        ./api/exports.def
)
set_target_properties(PyHP.EXE PROPERTIES OUTPUT_NAME pyhp)


include_directories(PyHP.EXE ${PYTHON_INCLUDE})
include_directories(PyHP.EXE ${PYHP_INCLUDE})

add_library(
        PyHP_API
        SHARED

        ./api/main.c
        ../../src/encoding.c
        ../../src/parser.c
        ../../src/compiler.c
        ../../src/executor.c
        ./api/exports.def
)
set_target_properties(PyHP_API PROPERTIES OUTPUT_NAME PyHP_API)

include_directories(PyHP_API ${PYTHON_INCLUDE})
include_directories(PyHP_API ${PYHP_INCLUDE})


add_library(
        PyHP_Core
        SHARED

        ../../src/modules/core.c
        ./core/parser.c
        ./core/compiler.c
        ./core/executor.c
        ./core/exports.def
)

add_dependencies(PyHP_Core PyHP_API)

set_target_properties(PyHP_Core PROPERTIES OUTPUT_NAME PyHP_Core)

include_directories(PyHP_Core ${PYHP_INCLUDE})

install(TARGETS PyHP.EXE DESTINATION out)