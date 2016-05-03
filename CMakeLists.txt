cmake_minimum_required(VERSION 3.1)
project(Bataille)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES server.c network.c network.h utils.c utils.h constants.h messages.h game.c game.h client.c client.h)
add_executable(Bataille ${SOURCE_FILES})