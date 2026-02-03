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

I chose not to implement variables into my assembler... Why?
Because I don't like them. They make code more confusing as telling the difference between labels and variables is impossible. I have included the built in variables `SCREEN` & `KBD` as they are necessary and because of this, adding variables to my assembler would not be too difficult. I've done this out of choice :p.

## Registers 

There are two registers, `D` & `A`. `D` stores data, `A` can store a constant or a pointer.
In the assembly language, there is a third register `M`, which is an alias of `*A`

