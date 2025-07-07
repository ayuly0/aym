#include "operand.h"

u8 operand_as_u8( Operand operand )
{
    switch ( operand.type )
    {
    case OPERAND_IMMEDIATE: return operand.imm.as_u64;
    case OPERAND_REGISTER : return operand.reg;
    case OPERAND_MEMORY   : return operand.mem_addr;
    default               : return OPERAND_NONE;
    }
}

Operand u8_as_operand( OperandType type, u8 value )
{
    Operand op = { .type = type };
    switch ( type )
    {
    case OPERAND_REGISTER : op.reg = value; break;
    case OPERAND_IMMEDIATE: op.imm.as_u64 = value; break;
    case OPERAND_MEMORY   : op.mem_addr = value; break;
    default               : break;
    }
    return op;
}
