#ifndef AYM_H
#define AYM_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "types.h"

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
    SYSCALL_EXIT = 0,
    SYSCALL_WRITE,
} VmSyscall;

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
    AYM_SUCCESS = 0,
    AYM_ERR_NULL_VM,
    AYM_ERR_NULL_PROGRAM,
    AYM_ERR_PROGRAM_TOO_LARGE,
    AYM_ERR_INVALID_SIZE,
    AYM_ERR_MEMCPY_FAILED,
} AYM_Status;

typedef enum
{
    INST_NOP = 0,
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
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER,
    OPERAND_MEMORY
} OperandType;

typedef struct
{
    OperandType type;
    union {
        int reg;
        Word imm;
        int mem_addr;
    };
} Operand;

typedef struct
{
    InstType type;
    Operand src;
    Operand dst;
} Inst;

typedef struct AYM_t
{
    Word stack[ AYM_MAX_STACK_SIZE ];

    Inst program[ AYM_MAX_PROGRAM_SIZE ];
    u64 program_size;

    Word registers[ REG_COUNT ];
    Word memory[ AYM_MAX_MEMORY_SIZE ];

    bool halt;
} AYM;

char *err_as_cstr( Err err );

char *inst_as_cstr( InstType inst_type );

void aym_init( AYM *vm );

Err aym_execute_inst( AYM *vm );

Err aym_execute_program( AYM *vm );

AYM_Status aym_load_program_from_mem( AYM *vm, Inst *program, size_t program_size );

Word aym_reslove_operand( AYM *vm, Operand operand );

void aym_dump_stack( FILE *stream, AYM *vm );

void aym_dump_register( FILE *stream, AYM *vm );

Err invoke_syscall( AYM *vm );

#endif // !AYM_H
