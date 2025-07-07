#include "syscall.h"
#include "aym.h"

Err invoke_syscall( AYM *vm )
{
    VmSyscall syscall_code = vm->stack[ --vm->registers[ REG_ESP ].as_u64 ].as_u64;
    switch ( syscall_code )
    {
    case SYSCALL_EXIT: {
        u64 code = vm->stack[ --vm->registers[ REG_ESP ].as_u64 ].as_u64;
        vm->halt = true;
        break;
    }
    case SYSCALL_WRITE: {
        u64 len     = vm->stack[ --vm->registers[ REG_ESP ].as_u64 ].as_u64;
        u64 buf_val = vm->stack[ --vm->registers[ REG_ESP ].as_u64 ].as_u64;
        u64 fd      = vm->stack[ --vm->registers[ REG_ESP ].as_u64 ].as_u64;
        void *buf   = ( void * )( uintptr_t )buf_val;
        write( fd, buf, len );
        break;
    }
    default: fprintf( stderr, "Unknown syscall %d\n", syscall_code ); return ERR_UNKOWN_SYSCALL;
    }
    return ERR_OK;
}
