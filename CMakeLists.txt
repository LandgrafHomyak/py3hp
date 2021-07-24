cmake_minimum_required(VERSION 3.19)
project(PyHP C)

set(CMAKE_C_STANDARD 90)

find_package(Python COMPONENTS Development)

if (NOT SKBUILD)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/venv/Lib/site-packages/skbuild/resources/cmake/)
endif ()


python_add_library(
        libPyHP
        SHARED

        src/parser.c
)
set_target_properties(libPyHP PROPERTIES OUTPUT_NAME pyhp)
target_include_directories(
        libPyHP

        PUBLIC
        Include
)

install(
        TARGETS libPyHP DESTINATION pyhp
)

python_add_library(
        python_pyhp_parser
        MODULE

        src/modules/parser.c
)
add_dependencies(python_pyhp_parser libPyHP)
target_link_libraries(python_pyhp_parser PRIVATE libPyHP)
set_target_properties(python_pyhp_parser PROPERTIES OUTPUT_NAME parser)


install(
        TARGETS python_pyhp_parser DESTINATION pyhp
)