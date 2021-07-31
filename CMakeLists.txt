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

        src/iterator_meta.c
#
#        src/statement_type.c
#        src/parser.c
#        src/_parser.cpp
#        src/compiler.c
)
set_target_properties(PyHP_api PROPERTIES OUTPUT_NAME pyhp)

target_include_directories(
        PyHP_api

        PUBLIC
        Include
)

install(
        TARGETS PyHP_api DESTINATION pyhp
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
        python_pyhp_types
        MODULE

        src/modules/types.c
)
add_dependencies(python_pyhp_types PyHP_api)
target_link_libraries(python_pyhp_types PRIVATE PyHP_api)
set_target_properties(python_pyhp_types PROPERTIES OUTPUT_NAME types)


install(
        TARGETS python_pyhp_parser DESTINATION pyhp
)