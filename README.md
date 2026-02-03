# Hack Computer 

This repository contains an emulator for the hack computer, as well as an assembler and disassembler.

## Build

To build run 

```
mkdir build 
cd build
cmake -S .. -B .
cmake --build .
```

The emulator depends on SDL2 which will be aquired using `fetchcontent`

## Details

For details on the hack computer architecture, read architecture.md

## Registers 

There are two registers, `D` & `A`. `D` stores data, `A` can store a constant or a pointer.
In the assembly language, there is a third register `M`, which is an alias of `*A`

