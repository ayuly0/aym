#ifndef AYM_H
#define AYM_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "types.h"

#define AYM_STACK_SIZE   1024 * 1024 // 1MB
#define AYM_PROGRAM_SIZE 1024 * 1024 // 1MB
#define AYM_MEMORY_SIZE  1024 * 1024 // 1MB

// vm->regs[flags] |= FLAG_ZERO;   -> set zero flag
// vm->regs[flags] &= ~FLAG_ZERO;  -> clear zero flag
#define FLAG_ZERO  ( 1 << 0 )
#define FLAG_SIGN  ( 1 << 1 )
#define FLAG_CARRY ( 1 << 2 )
#define FLAG_OVER  ( 1 << 3 )

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
    ERR_UNKOWN_SYSCALL,
} Err;

typedef enum
{
    INST_NOP = 0,
    INST_PUSH,
    INST_POP_STACK,
    INST_SWAP,
    INST_PLUS,
    INST_SUB_STACK,
    INST_DIV_STACK,
    INST_MUL_STACK,
    INST_DUP,

    INST_MOV,
    INST_ADD,
    INST_SUB_REG, // [ ]
    INST_DIV_REG, // [ ]
    INST_MUL_REG, // [ ]
    INST_POP_REG, // [ ]
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
    INST_JZ,
    INST_JNZ,
    INST_CALL,
    INST_RET,

    INST_SYSCALL,
    INST_HALT,
} InstType;

typedef enum
{
    REG_0 = 0,
    REG_1,
    REG_2,
    REG_3,
    REG_4,
    REG_5,
    REG_BP,
    REG_ESP,
    REG_IP,
    REG_FLAGS
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
    Word stack[ AYM_STACK_SIZE ];

    Inst program[ AYM_PROGRAM_SIZE ];
    u64 program_size;

    Word registers[ 10 ];
    Word memory[ AYM_MEMORY_SIZE ];

    bool halt;
} AYM;

char *err_as_cstr( Err err );

char *inst_as_cstr( InstType inst_type );

void aym_init( AYM *vm );

Err aym_execute_inst( AYM *vm );

Err aym_execute_program( AYM *vm );

Word aym_reslove_operand( AYM *vm, Operand operand );

void aym_dump_stack( FILE *stream, AYM *vm );

void aym_dump_register( FILE *stream, AYM *vm );

Err invoke_syscall( AYM *vm );

#endif // !AYM_H
