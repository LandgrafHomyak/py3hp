cmake_minimum_required(VERSION 3.19)
project(PyHP C CXX)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 98)

find_package(Python COMPONENTS Development)

if (NOT SKBUILD)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/venv/Lib/site-packages/skbuild/resources/cmake/)
endif ()

python_add_library(
        PyHP_api
        SHARED

        src/pyhp.def
        src/init.c
        src/py_functions.h

        src/iterator_meta.c

        src/statement_type.c
        src/parser_match.c
        src/parser.c
        src/parser.cpp

        src/prepare_result.c
        src/align_code.cpp
        src/prepare.c
        src/prepare.cpp

        src/command_type.c
        src/command.c
        src/compiler.c

        src/executor.c
)
set_target_properties(PyHP_api PROPERTIES OUTPUT_NAME pyhp)
target_include_directories(PyHP_api PUBLIC Include)

python_add_library(
        PyHP_api++
        STATIC

        src/pyhp++.def

        src/parser.cpp
        src/align_code.cpp
)
set_target_properties(PyHP_api++ PROPERTIES OUTPUT_NAME pyhp++)
target_link_libraries(PyHP_api++ PUBLIC PyHP_api)
target_link_libraries(PyHP_api INTERFACE PyHP_api++)

install(
        TARGETS PyHP_api PyHP_api++ DESTINATION pyhp
)

python_add_library(
        python_pyhp_parser
        MODULE

        src/modules/parser.c
)
add_dependencies(python_pyhp_parser PyHP_api)
target_link_libraries(python_pyhp_parser PRIVATE PyHP_api)
set_target_properties(python_pyhp_parser PROPERTIES OUTPUT_NAME parser)

python_add_library(
        python_pyhp_compiler
        MODULE

        src/modules/compiler.c
)
add_dependencies(python_pyhp_compiler PyHP_api)
target_link_libraries(python_pyhp_compiler PRIVATE PyHP_api)
set_target_properties(python_pyhp_compiler PROPERTIES OUTPUT_NAME compiler)

python_add_library(
        python_pyhp_executor
        MODULE

        src/modules/executor.c
)
add_dependencies(python_pyhp_executor PyHP_api)
target_link_libraries(python_pyhp_executor PRIVATE PyHP_api)
set_target_properties(python_pyhp_executor PROPERTIES OUTPUT_NAME executor)


set_target_properties(PyHP_api python_pyhp_parser python_pyhp_compiler python_pyhp_executor PROPERTIES LINKER_LANGUAGE C)


install(
        TARGETS python_pyhp_parser python_pyhp_compiler python_pyhp_executor DESTINATION pyhp
)
