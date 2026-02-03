// mem[5] = x
@10
D=A
@5
M=D

// Shift left

// IN:
//  mem[5]: x
// OUT:
//  mem[6]: x << 1

// setup bits
// mem[24] = 0x1 
// mem[25] = 0x2
// mem[26] = 0x4
// ...
@24
M=1

@2
D=A
@25
M=D

@4
D=A
@26
M=D

@8
D=A
@27
M=D

@16
D=A
@28
M=D

@32
D=A
@29
M=D

@64
D=A
@30
M=D

@128
D=A
@31
M=D

@256
D=A
@32
M=D

@512
D=A
@33
M=D

@1024
D=A
@34
M=D

@2048
D=A
@35
M=D

@4096
D=A
@36
M=D

@8192
D=A
@37
M=D

@16384
D=A
@38
M=D


@32767      // need to hack this one because
D=A+1       // if bit 16 of an A inst is set,
@39         // it will be interpreted as a 
M=D         // C inst. So set it to 0x7FFF 
            // and add 1.


            // This one is needed for generating
            // the output value, which will 
            // increment the pointer. Obviously in
            // the simulator it doesn't matter if 
@40         // it accesses an address I haven't touched,
M=0         // but it feels like good practice to
            // "Allocate" something here...


            // begin actual procedure
@0
D=A
@6
M=D         // set return value to 0

@16
D=A
@4
M=D         // set mem[4] to 16 as counter

(loop)
@40
D=A         // set d to pointer to end of bit table

@4
D=D-M       // subtract counter to get pointer to current bit

@3
M=D         // save current bit pointer to mem[3]

@5
D=M         // get the input value

@3
A=M         // get the current pointer into the bit table
D=D&M       // InputValue & BitTable[CurrentBit]
@loopend
D;JEQ

@3
A=M         // get the current pointer into the bit table... again
A=A+1       // increment the pointer by one
D=M         // D = BitTable[CurrentBit + 1]

@6
M=D|M

(loopend)
@4
M=M-1
D=M
@loop
D;JNE

