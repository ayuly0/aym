#include "aym.h"

char *err_as_cstr( Err err )
{
    char *err_str;
    switch ( err )
    {
    case ERR_STACK_OVERFLOW     : err_str = "STACK_OVERFLOW"; break;
    case ERR_STACK_UNDERFLOW    : err_str = "STACK_UNDERFLOW"; break;
    case ERR_ILLEGAL_INST_ACCESS: err_str = "ERR_ILLEGAL_INST_ACCESS"; break;
    case ERR_ILLEGAL_INST       : err_str = "ERR_ILLEGAL_INST"; break;
    case ERR_UNKOWN_SYSCALL     : err_str = "ERR_UNKOWN_SYSCALL"; break;
    default                     : err_str = "UNKOWN_ERR"; break;
    }
    return err_str;
}

void aym_init( AYM *vm )
{
    vm->stack_size   = 0;
    vm->program_size = 0;
    vm->ip           = 0;
    vm->halt         = false;

    memset( vm->stack, 0, AYM_STACK_SIZE );
    memset( vm->program, 0, AYM_PROGRAM_SIZE );
}

Err aym_execute_inst( AYM *vm )
{
    if ( vm->ip >= vm->program_size )
    {
        return ERR_ILLEGAL_INST_ACCESS;
    }

    Inst inst = vm->program[ vm->ip ];

    switch ( inst.type )
    {
    case INST_NOP: vm->ip++; break;
    case INST_PUSH:
        if ( vm->stack_size >= AYM_STACK_SIZE )
        {
            return ERR_STACK_OVERFLOW;
        }

        vm->stack[ vm->stack_size++ ] = inst.operand;
        vm->ip++;
        break;
    case INST_PLUS:
        if ( vm->stack_size < 2 )
        {
            return ERR_STACK_UNDERFLOW;
        }
        vm->stack[ vm->stack_size - 2 ].as_i64 =
            vm->stack[ vm->stack_size - 1 ].as_i64 + vm->stack[ vm->stack_size - 2 ].as_i64;
        vm->stack_size--;
        vm->ip++;
        break;
    case INST_SYSCALL:
        if ( handle_syscall( vm ) != ERR_OK )
        {
            return ERR_UNKOWN_SYSCALL;
        }
        vm->ip++;
        break;
    case INST_HALT: vm->halt = true; break;
    default       : printf( "INST: %d\n", inst.type ); return ERR_ILLEGAL_INST;
    }

    return ERR_OK;
}

Err aym_execute_program( AYM *vm )
{
    while ( !vm->halt )
    {
        aym_dump_stack( stdout, vm );
        Err err = aym_execute_inst( vm );
        if ( err != ERR_OK )
        {
            printf( "Err: %s\n", err_as_cstr( err ) );
            return err;
        }
    }
    return ERR_OK;
}

void aym_dump_stack( FILE *stream, AYM *vm )
{
    fprintf( stream, "Stack:\n" );
    if ( vm->stack_size > 0 )
    {
        for ( u64 i = 0; i < vm->stack_size; ++i )
        {
            fprintf(
                stream, "  u64: %" PRIu64 ", i64: %" PRId64 ", f64: %lf, ptr: %p\n", vm->stack[ i ].as_u64,
                vm->stack[ i ].as_i64, vm->stack[ i ].as_f64, vm->stack[ i ].as_ptr
            );
        }
    }
    else
    {
        fprintf( stream, "  [empty]\n" );
    }
}

Err handle_syscall( AYM *vm )
{
    VmSyscall syscall_code = vm->stack[ --vm->stack_size ].as_u64;
    switch ( syscall_code )
    {
    case SYSCALL_EXIT: {
        u64 code = vm->stack[ --vm->stack_size ].as_u64;
        printf( "VM exiting wth code %llu", code );
        vm->halt = true;
        break;
    }
    case SYSCALL_WRITE: {
        u64 len     = vm->stack[ --vm->stack_size ].as_u64;
        u64 buf_val = vm->stack[ --vm->stack_size ].as_u64;
        u64 fd      = vm->stack[ --vm->stack_size ].as_u64;
        void *buf   = ( void * )( uintptr_t )buf_val;
        write( fd, buf, len );
        break;
    }
    default: fprintf( stderr, "Unknown syscall %d\n", syscall_code ); return ERR_UNKOWN_SYSCALL;
    }
    return ERR_OK;
}
