# only for code hints and highlighting

cmake_minimum_required(VERSION 3.16)
project(core C)

set(CMAKE_C_STANDARD 99)

add_executable(core
        main.c

        code.c code.h
        code/class.c code/class.h
        code/commandtype.c code/commandtype.h
        code/highlevel.c code/highlevel.h
        code/iterator.c code/iterator.h
        code/parser.c code/parser.h
        )
# add_executable(test code/parser.c)
if (1)
    include_directories(_py3hp C:/Python/Python38-32/Include)
    #    include_directories(test C:/Python/Python38-32/Include)
    #    target_link_libraries(test C:/Python/Python38-32/python38.dll)
else ()
    include_directories(_py3hp C:/Python/Python32/Include)
    #    include_directories(test C:/Python/Python32/Include)
    #    target_link_libraries(test C:/Python/Python32/python32.dll)
endif ()