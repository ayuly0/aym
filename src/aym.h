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
    INST_STACK_NOP = 0,
    INST_STACK_PUSH,
    INST_STACK_POP,
    INST_STACK_SWAP,
    INST_STACK_PLUS,
    INST_STACK_SUB,
    INST_STACK_DIV,
    INST_STACK_MUL,
    INST_STACK_SYSCALL,
    INST_STACK_HALT,
} InstStackType;

typedef enum
{
    INST_REG_MOV = 0,
} InstRegisterType;

typedef enum
{
    INST_STACK = 0,
    INST_REG
} InstType;

typedef enum
{
    REG_0 = 0,
    REG_1,
    REG_2,
    REG_3,
    REG_4,
    REG_5,
    REG_BP,  // -> base_func_addr
    REG_ESP, // -> stack_size
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
        int reg;      // REGISTER
        Word imm;     // IMMEDIATE
        int mem_addr; // MEMORY
    };
} Operand;

typedef struct
{
    InstType type;
    int opcode;
    Operand src;
    Operand dst;
} Inst;

typedef struct AYM_t
{
    Word stack[ AYM_STACK_SIZE ];

    Inst program[ AYM_PROGRAM_SIZE ];
    u64 program_size;

    Word registers[ 10 ];

    bool halt;
} AYM;

char *err_as_cstr( Err err );

char *inst_as_cstr( InstType inst_type, int inst_opcode );

void aym_init( AYM *aym );

Err aym_execute_inst( AYM *aym );

Err aym_execute_program( AYM *aym );

void aym_dump_stack( FILE *stream, AYM *vm );

void aym_dump_register( FILE *stream, AYM *vm );

Err invoke_syscall( AYM *vm );

#endif // !AYM_H
