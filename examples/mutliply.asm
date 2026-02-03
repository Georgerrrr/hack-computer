// Multiply

// IN:
//  mem[5]: x
//  mem[6]: y 
// OUT:
//  mem[7]: x * y

@4 
D=A
@5
M=D

// Mem[6] = 2
@2
D=A
@6
M=D

// Mem[7] = out
@7
M=0

(loop)
@5
D=M // load mem[5]

@7
M=D+M // mem[7] = mem[5] + mem[7]

@6 
M=M-1 // mem[6]-1
D=M // load mem[6]
@loop
D;JNE

(end)
@end 
0;JMP
