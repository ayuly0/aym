#include "operand.h"

u8 operand_as_u8( Operand operand )
{
    switch ( operand.type )
    {
    case OPERAND_IMMEDIATE: return ( u8 )operand.imm.as_u64;
    case OPERAND_REGISTER : return ( u8 )operand.reg;
    case OPERAND_MEMORY   : return ( u8 )operand.mem_addr;
    default               : return OPERAND_NONE;
    }
}

u32 operand_as_u32( Operand operand )
{
    switch ( operand.type )
    {
    case OPERAND_IMMEDIATE: return ( u32 )operand.imm.as_u64;
    case OPERAND_REGISTER : return operand.reg;
    case OPERAND_MEMORY   : return operand.mem_addr;
    default               : return OPERAND_NONE;
    }
}

u64 operand_as_u64( Operand operand )
{
    switch ( operand.type )
    {
    case OPERAND_IMMEDIATE: return ( u64 )operand.imm.as_u64;
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

Operand u32_as_operand( OperandType type, u32 value )
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

Operand u64_as_operand( OperandType type, u64 value )
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
