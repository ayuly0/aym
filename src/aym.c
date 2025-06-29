#include "aym.h"

char *err_as_cstr( Err err )
{
    switch ( err )
    {
    case ERR_STACK_OVERFLOW     : return "STACK_OVERFLOW";
    case ERR_STACK_UNDERFLOW    : return "STACK_UNDERFLOW";
    case ERR_ILLEGAL_INST_ACCESS: return "ERR_ILLEGAL_INST_ACCESS";
    case ERR_ILLEGAL_INST       : return "ERR_ILLEGAL_INST";
    case ERR_DIV_BY_ZERO        : return "ERR_DIV_BY_ZERO";
    case ERR_UNKOWN_SYSCALL     : return "ERR_UNKOWN_SYSCALL";
    default                     : return "UNKOWN_ERR";
    }
}

char *inst_as_cstr( InstType inst_type, int inst_opcode )
{
    if ( inst_type == INST_STACK )
    {
        switch ( inst_opcode )
        {
        case INST_STACK_NOP    : return "INST_STACK_NOP";
        case INST_STACK_PUSH   : return "INST_STACK_PUSH";
        case INST_STACK_POP    : return "INST_STACK_POP";
        case INST_STACK_SWAP   : return "INST_STACK_SWAP";
        case INST_STACK_PLUS   : return "INST_STACK_PLUS";
        case INST_STACK_SUB    : return "INST_STACK_SUB";
        case INST_STACK_DIV    : return "INST_STACK_DIV";
        case INST_STACK_MUL    : return "INST_STACK_MUL";
        case INST_STACK_SYSCALL: return "INST_STACK_SYSCALL";
        case INST_STACK_HALT   : return "INST_STACK_HALT";
        default                : return "UNKOWN_INST_STACK";
        }
    }

    // INST_REGISTER
    switch ( inst_opcode )
    {

    default: return "UNKOWN_INST_REG";
    }
}

void aym_init( AYM *vm )
{
    vm->registers[ REG_0 ].as_u64     = 0;
    vm->registers[ REG_1 ].as_u64     = 0;
    vm->registers[ REG_2 ].as_u64     = 0;
    vm->registers[ REG_3 ].as_u64     = 0;
    vm->registers[ REG_4 ].as_u64     = 0;
    vm->registers[ REG_5 ].as_u64     = 0;
    vm->registers[ REG_BP ].as_u64    = 0;
    vm->registers[ REG_ESP ].as_u64   = 0;
    vm->registers[ REG_IP ].as_u64    = 0;
    vm->registers[ REG_FLAGS ].as_u64 = 0;
    vm->program_size                  = 0;
    vm->halt                          = false;

    memset( vm->stack, 0, AYM_STACK_SIZE );
    memset( vm->program, 0, AYM_PROGRAM_SIZE );
}

Err aym_execute_inst( AYM *vm )
{
    if ( vm->registers[ REG_IP ].as_u64 >= vm->program_size )
    {
        return ERR_ILLEGAL_INST_ACCESS;
    }

    Inst inst = vm->program[ vm->registers[ REG_IP ].as_u64 ];

    if ( inst.type == INST_STACK )
    {
        switch ( inst.opcode )
        {
        case INST_STACK_NOP: vm->registers[ REG_IP ].as_u64++; break;

        case INST_STACK_PUSH:
            if ( vm->registers[ REG_ESP ].as_u64 >= AYM_STACK_SIZE )
            {
                return ERR_STACK_OVERFLOW;
            }
            vm->stack[ vm->registers[ REG_ESP ].as_u64++ ] = inst.src.imm;
            vm->registers[ REG_IP ].as_u64++;
            break;

        case INST_STACK_POP:
            if ( vm->registers[ REG_ESP ].as_u64 <= 0 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            vm->registers[ REG_ESP ].as_u64--;
            vm->registers[ REG_IP ].as_u64++;
            break;

        case INST_STACK_SWAP:
            if ( vm->registers[ REG_ESP ].as_u64 < 2 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            i64 temp                                         = vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ].as_i64;
            vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ] = vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ];
            vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 = temp;
            break;

        case INST_STACK_PLUS:
            if ( vm->registers[ REG_ESP ].as_u64 < 2 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 =
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ].as_i64 +
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64;
            vm->registers[ REG_ESP ].as_u64--;
            vm->registers[ REG_IP ].as_u64++;
            break;

        case INST_STACK_SUB:
            if ( vm->registers[ REG_ESP ].as_u64 < 2 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 =
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 -
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ].as_i64;
            vm->registers[ REG_ESP ].as_u64--;
            vm->registers[ REG_IP ].as_u64++;
            break;

        case INST_STACK_DIV:
            if ( vm->registers[ REG_ESP ].as_u64 < 2 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            if ( vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ].as_i64 == 0 )
            {
                return ERR_DIV_BY_ZERO;
            }
            vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 =
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 /
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ].as_i64;
            vm->registers[ REG_ESP ].as_u64--;
            vm->registers[ REG_IP ].as_u64++;
            break;

        case INST_STACK_MUL:
            if ( vm->registers[ REG_ESP ].as_u64 < 2 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 =
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 *
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ].as_i64;
            vm->registers[ REG_ESP ].as_u64--;
            vm->registers[ REG_IP ].as_u64++;
            break;

        case INST_STACK_SYSCALL:
            if ( invoke_syscall( vm ) != ERR_OK )
            {
                return ERR_UNKOWN_SYSCALL;
            }
            vm->registers[ REG_IP ].as_u64++;
            break;
        case INST_STACK_HALT:
            printf( "Exiting..\n" );
            vm->halt = true;
            break;
        default:
            printf(
                "ILLEGAL INST: %s, TYPE: %d, OPCODE: %d\n", inst_as_cstr( inst.type, inst.opcode ), inst.type,
                inst.opcode
            );
            return ERR_ILLEGAL_INST;
        }
    }
    else
    {
        // INST_REGISTER
        switch ( inst.opcode )
        {
        case INST_REG_MOV: {
            Word value;
            if ( inst.src.type == OPERAND_REGISTER )
            {
                value = vm->registers[ inst.src.reg ];
            }
            else if ( inst.src.type == OPERAND_IMMEDIATE )
            {
                value = inst.src.imm;
            }
            if ( inst.dst.type == OPERAND_REGISTER )
            {
                vm->registers[ inst.dst.reg ] = value;
            }
            vm->registers[ REG_IP ].as_u64++;
            break;
        }

        default:
            printf(
                "ILLEGAL INST: %s, TYPE: %d, OPCODE: %d\n", inst_as_cstr( inst.type, inst.opcode ), inst.type,
                inst.opcode
            );
            return ERR_ILLEGAL_INST;
        }
    }

    return ERR_OK;
}

Err aym_execute_program( AYM *vm )
{
    while ( !vm->halt )
    {
        aym_dump_register( stdout, vm );
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
    if ( vm->registers[ REG_ESP ].as_u64 > 0 )
    {
        for ( u64 i = 0; i < vm->registers[ REG_ESP ].as_u64; ++i )
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

void aym_dump_register( FILE *stream, AYM *vm )
{
    fprintf( stream, "Registers:\n" );
    fprintf(
        stream,
        "  Reg0: %p, Reg1: %p, Reg2: %p\n  Reg3: %p, Reg4: %p, Reg5: %p\n  Bp  : %p, Esp : %p, Ip  : %p\n  Flags: %p\n",
        vm->registers[ REG_0 ].as_ptr, vm->registers[ REG_1 ].as_ptr, vm->registers[ REG_2 ].as_ptr,
        vm->registers[ REG_3 ].as_ptr, vm->registers[ REG_4 ].as_ptr, vm->registers[ REG_5 ].as_ptr,
        vm->registers[ REG_BP ].as_ptr, vm->registers[ REG_ESP ].as_ptr, vm->registers[ REG_IP ].as_ptr,
        vm->registers[ REG_FLAGS ].as_ptr
    );
}

Err invoke_syscall( AYM *vm )
{
    VmSyscall syscall_code = vm->stack[ --vm->registers[ REG_ESP ].as_u64 ].as_u64;
    switch ( syscall_code )
    {
    case SYSCALL_EXIT: {
        u64 code = vm->stack[ --vm->registers[ REG_ESP ].as_u64 ].as_u64;
        printf( "VM exiting wth code %llu", code );
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
