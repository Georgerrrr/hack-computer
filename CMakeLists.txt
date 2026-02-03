cmake_minimum_required(VERSION 3.30)

project(hack-computer LANGUAGES CXX)

add_subdirectory(3rdparty)
add_subdirectory(common)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

add_subdirectory(emulator)
add_subdirectory(assembler)
add_subdirectory(disassembler)

file(CREATE_LINK
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_SOURCE_DIR}/compile_commands.json"
  SYMBOLIC
)

