cmake_minimum_required (VERSION 2.8)

project (MQ)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread -Wl,--no-as-needed -O3 -g -ggdb -DDEBUG -D_DEBUG")

aux_source_directory(. DIR_SRC)

add_executable(MQ ${DIR_SRC})