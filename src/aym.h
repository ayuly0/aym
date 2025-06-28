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
    ERR_UNKOWN_SYSCALL,
} Err;

typedef enum
{
    INST_NOP = 0,
    INST_PUSH,
    INST_PLUS,
    INST_HALT,
    INST_SYSCALL,
} InstType;

typedef struct
{
    InstType type;
    Word operand;
} Inst;

typedef struct AYM_t
{
    Word stack[ AYM_STACK_SIZE ];
    u64 stack_size;

    Inst program[ AYM_PROGRAM_SIZE ];
    u64 program_size;

    u64 ip;

    bool halt;
} AYM;

char *err_as_cstr( Err err );

void aym_init( AYM *aym );

Err aym_execute_inst( AYM *aym );

Err aym_execute_program( AYM *aym );

void aym_dump_stack( FILE *stream, AYM *vm );

Err handle_syscall( AYM *vm );

#endif // !AYM_H
