#ifndef DEFS_H_
#define DEFS_H_

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int8_t i8;

typedef union {
    u64 as_u64;
    i64 as_i64;
    double as_f64;
    void *as_ptr;
} Word;

#define AYM_MAX_STACK_SIZE   1024 * 1024 // 1MB
#define AYM_MAX_PROGRAM_SIZE 1024 * 1024 // 1MB
#define AYM_MAX_MEMORY_SIZE  1024 * 1024 // 1MB

// vm->regs[flags] |= FLAG_ZERO;   -> set zero flag
// vm->regs[flags] &= ~FLAG_ZERO;  -> clear zero flag
#define FLAG_ZERO     ( 1 << 0 )
#define FLAG_SIGN     ( 1 << 1 )
#define FLAG_CARRY    ( 1 << 2 )
#define FLAG_OVERFLOW ( 1 << 3 )

typedef enum
{
    ERR_OK = 0,
    ERR_STACK_OVERFLOW,
    ERR_STACK_UNDERFLOW,
    ERR_ILLEGAL_INST,
    ERR_ILLEGAL_INST_ACCESS,
    ERR_DIV_BY_ZERO,
    ERR_ILLEGAL_OPERAND_TYPE,
    ERR_UNKOWN_SYSCALL,
} Err;

typedef enum
{
    AYM_OK = 0,
    AYM_ERR_NULL_VM,
    AYM_ERR_NULL_PROGRAM,
    AYM_ERR_PROGRAM_TOO_LARGE,
    AYM_ERR_INVALID_SIZE,
    AYM_ERR_MEMCPY_FAILED,
    AYM_ERR_FILE_NOT_FOUND,
    AYM_ERR_INVALID_FORMAT,
    AYM_ERR_ALLOC_FAILED,
    AYM_ERR_IO,
    AYM_FILE_ERROR,
    AYM_WRITE_ERROR,
    AYM_INVALID_ARGUMENT,
} AYM_Status;

typedef enum
{
    INST_NOP = 0x00,
    INST_PUSH,
    INST_SWAP,
    INST_PLUS,
    INST_DUP,

    INST_SUB,
    INST_DIV,
    INST_MUL,
    INST_POP,

    INST_MOV,
    INST_ADD,
    INST_XOR,
    INST_AND,
    INST_NOT,
    INST_OR,
    INST_LEA,
    INST_CMP,
    INST_TEST,
    INST_LOAD,
    INST_STORE,

    INST_JMP,
    INST_JE,
    INST_JNE,
    INST_JG,
    INST_JL,
    INST_JGE,
    INST_JLE,
    INST_JA,
    INST_JB,
    INST_JAE,
    INST_JBE,
    INST_INC,
    INST_CALL,
    INST_RET,

    INST_SYSCALL,
    INST_HALT,
} InstType;

typedef enum
{
    REG_0 = 1,
    REG_1,
    REG_2,
    REG_3,
    REG_4,
    REG_5,
    REG_6,
    REG_7,
    REG_8,
    REG_9,
    REG_BP,
    REG_ESP,
    REG_IP,
    REG_FLAGS,

    REG_COUNT
} RegisterType;

typedef enum
{
    OPERAND_NONE = 0,
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER,
    OPERAND_MEMORY
} OperandType;

#endif // !DEFS_H_
