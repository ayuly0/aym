#include "util.h"
#include "defs.h"

char *err_as_cstr( Err err )
{
    switch ( err )
    {
    case ERR_STACK_OVERFLOW      : return "STACK_OVERFLOW";
    case ERR_STACK_UNDERFLOW     : return "STACK_UNDERFLOW";
    case ERR_ILLEGAL_INST_ACCESS : return "ERR_ILLEGAL_INST_ACCESS";
    case ERR_ILLEGAL_INST        : return "ERR_ILLEGAL_INST";
    case ERR_DIV_BY_ZERO         : return "ERR_DIV_BY_ZERO";
    case ERR_ILLEGAL_OPERAND_TYPE: return "ERR_ILLEGAL_OPERAND_TYPE";
    case ERR_UNKOWN_SYSCALL      : return "ERR_UNKNOWN_SYSCALL";
    default                      : return "UNKNOWN_ERR";
    }
}

char *aym_status_as_cstr( AYM_Status status )
{
    switch ( status )
    {
    case AYM_OK                   : return "AYM_OK";
    case AYM_ERR_NULL_VM          : return "AYM_ERR_NULL_VM";
    case AYM_ERR_NULL_PROGRAM     : return "AYM_ERR_NULL_PROGRAM";
    case AYM_ERR_PROGRAM_TOO_LARGE: return "AYM_ERR_PROGRAM_TOO_LARGE";
    case AYM_ERR_INVALID_SIZE     : return "AYM_ERR_INVALID_SIZE";
    case AYM_ERR_MEMCPY_FAILED    : return "AYM_ERR_MEMCPY_FAILED";
    case AYM_ERR_FILE_NOT_FOUND   : return "AYM_ERR_FILE_NOT_FOUND";
    case AYM_ERR_INVALID_FORMAT   : return "AYM_ERR_INVALID_FORMAT";
    case AYM_ERR_ALLOC_FAILED     : return "AYM_ERR_ALLOC_FAILED";
    case AYM_ERR_IO               : return "AYM_ERR_IO";
    case AYM_FILE_ERROR           : return "AYM_FILE_ERROR";
    case AYM_WRITE_ERROR          : return "AYM_WRITE_ERROR";
    case AYM_READ_ERROR           : return "AYM_READ_ERROR";
    case AYM_INVALID_ARGUMENT     : return "AYM_INVALID_ARGUMENT";
    case AYM_BAD_HEADER           : return "AYM_BAD_HEADER";
    case AYM_CHECKSUM_MISMATCH    : return "AYM_CHECKSUM_MISMATCH";
    default                       : return "UNKNOWN_AYM_STATUS";
    }
}

char *operand_type_as_cstr( OperandType op_type )
{
    switch ( op_type )
    {
    case OPERAND_IMMEDIATE: return "OPERAND_IMMEDIATE";
    case OPERAND_REGISTER : return "OPERAND_REGISTER";
    case OPERAND_MEMORY   : return "OPERAND_MEMORY";
    case OPERAND_LABEL    : return "OPERAND_LABEL";
    case OPERAND_NONE     : return "OPERAND_NONE";
    default               : return "Unreachable!";
    }
}
