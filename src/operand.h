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
        char *label_name;
    };
} Operand;

u8 operand_as_u8( Operand operand );
Operand u8_as_operand( OperandType type, u8 value );

u32 operand_as_u32( Operand operand );
Operand u32_as_operand( OperandType type, u32 value );

u64 operand_as_u64( Operand operand );
Operand u64_as_operand( OperandType type, u64 value );

#endif // !OPERAND_H_
