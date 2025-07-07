#ifndef AYM_H_
#define AYM_H_

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "defs.h"
#include "inst.h"
#include "operand.h"
#include "syscall.h"
#include "util.h"

typedef struct AYM_t
{
    Word stack[ AYM_MAX_STACK_SIZE ];

    Inst program[ AYM_MAX_PROGRAM_SIZE ];
    u64 program_size;

    Word registers[ REG_COUNT ];
    Word memory[ AYM_MAX_MEMORY_SIZE ];

    bool halt;
} AYM;

void aym_init( AYM *vm );

Err aym_execute_inst( AYM *vm );

Err aym_execute_program( AYM *vm );

Word aym_reslove_operand( AYM *vm, Operand operand );

void aym_dump_stack( FILE *stream, AYM *vm );

void aym_dump_register( FILE *stream, AYM *vm );

#endif // !AYM_H_
