#ifndef OPERAND_H_
#define OPERAND_H_

#include "defs.h"

typedef struct
{
    OperandType type;
    union {
        int reg;
        Word imm;
        int mem_addr;
    };
} Operand;

u8 operand_as_u8( Operand operand );

Operand u8_as_operand( OperandType type, u8 value );

#endif // !OPERAND_H_
