# only for code hints and highlighting

cmake_minimum_required(VERSION 3.16)
project(__init__ C)

set(CMAKE_C_STANDARD 11)

add_executable(
        core

        main.c
)
include_directories(__init__ ./../core)

# add_executable(test code/parser.c)
if (0)
    include_directories(__init__ C:/Python/Python38-32/Include)
    #    include_directories(test C:/Python/Python38-32/Include)
    #    target_link_libraries(test C:/Python/Python38-32/python38.dll)
else ()
    include_directories(__init__ C:/Python/Include/cpython-3.0)
    #    include_directories(test C:/Python/Python32/Include)
    #    target_link_libraries(test C:/Python/Python32/python32.dll)
endif ()