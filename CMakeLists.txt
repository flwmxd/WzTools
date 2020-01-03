cmake_minimum_required(VERSION 3.5)

project(WzTools)

get_filename_component(SRC_DIR
                       ${CMAKE_SOURCE_DIR}
                       ABSOLUTE)

get_filename_component(LIB_DIR
                       ${CMAKE_SOURCE_DIR}/lib
                       ABSOLUTE)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++14")

file(GLOB WZ_SRC 
    ${SRC_DIR}/*.cpp
    ${SRC_DIR}/*.h
)

file(GLOB Z_LIB 
    ${LIB_DIR}/zlib/src/*.cpp
    ${LIB_DIR}/zlib/src/*.h
)

include_directories(
    ${LIB_DIR}/zlib/src
)

add_library(WzTools STATIC ${WZ_SRC} ${Z_LIB})

