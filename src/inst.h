#ifndef INST_H_
#define INST_H_

#include "defs.h"
#include "operand.h"

typedef struct
{
    InstType type;
    Operand src;
    Operand dst;
} Inst;

char *inst_as_cstr( InstType inst_type );

#endif // !INST_H_
