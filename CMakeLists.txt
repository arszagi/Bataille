cmake_minimum_required(VERSION 3.1)
project(Bataille)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(DEPENDENCIES network.c network.h utils.c utils.h constants.h messages.h game.c game.h shared_memory.c shared_memory.h)
add_executable(Server ${DEPENDENCIES} server.c)
add_executable(Client ${DEPENDENCIES} client.h client.c)