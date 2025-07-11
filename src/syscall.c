#include "syscall.h"
#include "aym.h"

#include <stdlib.h>

Err invoke_syscall( AYM *vm )
{
    VmSyscall syscall_code = ( VmSyscall )vm->registers[ REG_0 ].as_u64;
    switch ( syscall_code )
    {
    case SYSCALL_EXIT: {
        u64 code = vm->registers[ REG_1 ].as_u64;
        vm->halt = true;
        break;
    }
    case SYSCALL_WRITE: {
        u64 fd    = vm->registers[ REG_1 ].as_u64;
        void *buf = vm->registers[ REG_2 ].as_ptr;
        u64 len   = vm->registers[ REG_3 ].as_u64;
        write( ( int )fd, buf, len );
        break;
    }
    case SYSCALL_READ: {
        u64 fd    = vm->registers[ REG_1 ].as_u64;
        void *buf = vm->registers[ REG_2 ].as_ptr;
        u64 len   = vm->registers[ REG_3 ].as_u64;
        read( ( int )fd, buf, len );
        break;
    }
    case SYSCALL_ALLOC: {
        u64 len                       = vm->registers[ REG_1 ].as_u64;
        vm->registers[ REG_2 ].as_ptr = malloc( len );
        break;
    }
    case SYSCALL_FREE: {
        void *ptr = vm->registers[ REG_1 ].as_ptr;
        free( ptr );
        break;
    }
    default: fprintf( stderr, "Unknown syscall %d\n", syscall_code ); return ERR_UNKOWN_SYSCALL;
    }
    return ERR_OK;
}
