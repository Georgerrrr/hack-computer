// Divide

// IN:
//  mem[5]: x
//  mem[6]: y 
// OUT:
//  mem[7]: x / y
//  mem[5]: x % y 

// mem[5] = x
@10
D=A
@5
M=D

// Mem[6] = y
@3
D=A
@6
M=D

// Mem[7] = div
@7
M=0

(loop)
@5
D=M // load mem[5]

@7 
M=M+1 // inc div output
@6
D=D-M // d = mem[5] - mem[6]
@5
M=D // set mem[5] to the value at D
@loop
D;JGT


@normd 
D;JEQ // if no remainder, skip...
// ... otherwise

@7
M=M-1
@6
D=M 
@5
M=D+M 
(normd)

