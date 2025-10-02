cmake_minimum_required(VERSION 3.5)

project(
        tjames
        LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(tjames src/main.c)
