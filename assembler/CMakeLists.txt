cmake_minimum_required(VERSION 3.30)

project(hack-assembler LANGUAGES CXX)
add_executable(hack-assembler main.cpp ${COMMON_SOURCES}) 
target_include_directories(hack-assembler PUBLIC ${CMAKE_SOURCE_DIR})
