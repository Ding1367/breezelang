cmake_minimum_required(VERSION 3.29)
project(breezelang C)

set(CMAKE_C_STANDARD 11)
add_compile_definitions(-DPARSER_DEBUG)

add_executable(breezelang main.c
        parser.c
        parser.h
        lexer.c
        lexer.h)
