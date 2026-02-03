cmake_minimum_required(VERSION 3.30)

project(hack-disassembler LANGUAGES CXX)
add_executable(hack-disassembler main.cpp ${COMMON_SOURCES})
target_include_directories(hack-disassembler PUBLIC ${CMAKE_SOURCE_DIR})

