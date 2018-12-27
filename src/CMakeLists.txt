cmake_minimum_required(VERSION 3.12)
project(lcjson C)

set(CMAKE_C_STANDARD 11)
set(SRC_LIST test.c lcjson.c)

add_executable(lcjson ${SRC_LIST})