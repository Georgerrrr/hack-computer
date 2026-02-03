## Instruction Set 

The hack computer uses two types of instruction, C instructions and A instructions.

### C instructions

A C instruction can do one of 18 operations:

| Instruction | Operation                           |
| ----------- | ----------------------------------- |
| 0           | Output 0                            |
| 1           | Output 1                            |
| -1          | Output -1                           |
| D           | Output D                            |
| A           | Output A                            |
| M           | Output Ram[A]                       |
| !D          | Output the flipped bits of D        | 
| !A          | Output the flipped bits of A        |
| !M          | Output the flipped bits of Ram[A]   |
| -D          | Output the inverse of D             |
| -A          | Output the inverse of A             |
| -M          | Output the inverse of Ram[A]        |
| D+1         | Output D+1                          |
| A+1         | Output A+1                          |
| M+1         | Output Ram[A]+1                     |
| D-1         | Output D-1                          |
| A-1         | Output A-1                          |
| M-1         | Output Ram[A]-1                     |
| D+A         | Output D+A                          |
| D+M         | Output D+Ram[A]                     |
| D-A         | Output D-A                          |
| D-M         | Output D-Ram[A]                     |
| A-D         | Output A-D                          |
| M-D         | Output Ram[A]-D                     |
| D&A         | Output D and A                      |
| D&M         | Output D and Ram[A]                 |
| D|A         | Output D or A                       |
| D|M         | Output D or Ram[A]                  |

They can store to 
 - `D`
 - `A`
 - `Ram[A]`

An operation can be stored to up to all three of these at once. Beware of storing to `A` and `Ram[A]` at once.

A C instruction can also do a jump based on the output of the operation.

| Jump | Operation                                       |
| ---- | ----------------------------------------------- |
| JGT  | Jump if operation is greater than 0             |
| JEQ  | Jump if the operation is equal to 0             |
| JGE  | Jump if operation is greater than or equal to 0 |
| JLT  | Jump if operation is less than 0                | 
| JNE  | Jump if operation doesn't equal 0               | 
| JLE  | Jump if operation is less than or equal to 0    |
| JMP  | Jump unconditionally                            |

### A instructions 

C instructions can do one of three things
 - set a register to a constant value of -1, 0, or 1,
 - set a register to another register, or negate another register
 - set a register to an operation on two registers, valid ops are (+, -, &, |) 

#### Syntax

  @const 

  A <- const

#### Examples

```
; set to constants
D=0
A=-1
M=1

; set to other register
D=A
D=-M

; operation 
D=D+A
```

