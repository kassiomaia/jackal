# Jackal IR

Jackal IR is the representation of the code in a linear structure
to be executed by the VM.

# Jackal IR Types

IR TYPE         | HEX    | ARITY | | TYPE  |
--------------------------------------------
JKL_IR_NOPE     [0x00ca] 0000 0000 1100 1010
JKL_IR_ALLOC    [0x01cb] 0000 0001 1100 1011
JKL_IR_LOAD     [0x01cc] 0000 0001 1100 1100
JKL_IR_STORE    [0x01cd] 0000 0001 1100 1101
JKL_IR_PUSHI    [0x01ce] 0000 0001 1100 1110
JKL_IR_PUSHF    [0x01cf] 0000 0001 1100 1111
JKL_IR_PUSHS    [0x01d0] 0000 0001 1101 0001
JKL_IR_POPI     [0x01d1] 0000 0001 1101 0001
JKL_IR_POPF     [0x01d2] 0000 0001 1101 0010
JKL_IR_POPS     [0x01d3] 0000 0001 1101 0011
JKL_IR_ADD      [0x00d4] 0000 0000 1101 0100
JKL_IR_SUB      [0x00d5] 0000 0000 1101 0101
JKL_IR_MUL      [0x00d6] 0000 0000 1101 0110
JKL_IR_DIV      [0x00d7] 0000 0000 1101 0111
JKL_IR_MOD      [0x00d8] 0000 0000 1101 1000
JKL_IR_EQL      [0x00d9] 0000 0000 1101 1001
JKL_IR_NEQ      [0x00da] 0000 0000 1101 1010
JKL_IR_GT       [0x00db] 0000 0000 1101 1011
JKL_IR_GTE      [0x00dc] 0000 0000 1101 1100
JKL_IR_LT       [0x00dd] 0000 0000 1101 1101
JKL_IR_LTE      [0x00de] 0000 0000 1101 1110
JKL_IR_AND      [0x00df] 0000 0000 1101 1111
JKL_IR_OR       [0x00e0] 0000 0000 1110 0000
JKL_IR_NOT      [0x00e1] 0000 0000 1110 0001
JKL_IR_JMP      [0x01e2] 0000 0001 1110 0010
JKL_IR_JCP      [0x01e3] 0000 0001 1110 0011
JKL_IR_CALL     [0x01e4] 0000 0001 1110 0100
JKL_IR_RET      [0x00e5] 0000 0000 1110 0101
JKL_IR_HALT     [0x00e6] 0000 0000 1110 0110
