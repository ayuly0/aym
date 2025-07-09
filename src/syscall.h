#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "defs.h"

typedef struct AYM_t AYM;

typedef enum
{
    SYSCALL_EXIT = 0,
    SYSCALL_WRITE,
    SYSCALL_READ,
    SYSCALL_ALLOC,
    SYSCALL_FREE,
} VmSyscall;

Err invoke_syscall( AYM *vm );

#endif // !SYSCALL_H_
