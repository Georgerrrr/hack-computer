cmake_minimum_required(VERSION 3.30)

project(hack-computer LANGUAGES CXX)

list(APPEND EMU_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/computer.cpp
)

add_executable(hack-computer ${EMU_SOURCES} ${COMMON_SOURCES})

target_link_libraries(hack-computer PRIVATE SDL2) 
target_include_directories(hack-computer PUBLIC SDL2 ${CMAKE_SOURCE_DIR})
