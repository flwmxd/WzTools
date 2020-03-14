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

file(GLOB ZLIB_SRC 
	${LIB_DIR}/zlib/src/*.c
    ${LIB_DIR}/zlib/src/*.h
)

file(GLOB TEST_SRC 
    ${SRC_DIR}/main.cpp
)


include_directories(
    ${LIB_DIR}/zlib/src
)

add_library(WzTools STATIC ${WZ_SRC} ${ZLIB_SRC})

set_target_properties(WzTools PROPERTIES COMPILE_FLAGS "/MP /wd4819 /arch:SSE")

add_executable(Test ${TEST_SRC})

target_include_directories(Test PRIVATE ${WZ_SRC})

target_link_libraries(Test WzTools)