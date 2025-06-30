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

char *inst_as_cstr( InstType inst_type )
{
    switch ( inst_type )
    {
    case INST_NOP      : return "INST_NOP";
    case INST_PUSH     : return "INST_PUSH";
    case INST_POP_STACK: return "INST_POP_STACK";
    case INST_SWAP     : return "INST_SWAP";
    case INST_PLUS     : return "INST_PLUS";
    case INST_SUB_STACK: return "INST_SUB_STACK";
    case INST_DIV_STACK: return "INST_DIV_STACK";
    case INST_MUL_STACK: return "INST_MUL_STACK";
    case INST_DUP      : return "INST_DUP";

    case INST_MOV    : return "INST_MOV";
    case INST_ADD    : return "INST_ADD";
    case INST_SUB_REG: return "INST_SUB_REG";
    case INST_DIV_REG: return "INST_DIV_REG";
    case INST_MUL_REG: return "INST_MUL_REG";
    case INST_POP_REG: return "INST_POP_REG";
    case INST_XOR    : return "INST_XOR";
    case INST_AND    : return "INST_AND";
    case INST_NOT    : return "INST_NOT";
    case INST_OR     : return "INST_OR";
    case INST_LEA    : return "INST_LEA";
    case INST_CMP    : return "INST_CMP";
    case INST_TEST   : return "INST_TEST";
    case INST_LOAD   : return "INST_LOAD";
    case INST_STORE  : return "INST_STORE";

    case INST_JMP : return "INST_JMP";
    case INST_JZ  : return "INST_JZ";
    case INST_JNZ : return "INST_JNZ";
    case INST_CALL: return "INST_CALL";
    case INST_RET : return "INST_RET";

    case INST_SYSCALL: return "INST_SYSCALL";
    case INST_HALT   : return "INST_HALT";
    default          : return "UNKOWN_INST";
    }
}

void aym_init( AYM *vm )
{
    vm->halt = false;

    memset( vm->stack, 0, AYM_STACK_SIZE );
    memset( vm->memory, 0, AYM_MEMORY_SIZE );
    memset( vm->program, 0, AYM_PROGRAM_SIZE );
}

Err aym_execute_inst( AYM *vm )
{
    if ( vm->registers[ REG_IP ].as_u64 >= vm->program_size )
    {
        return ERR_ILLEGAL_INST_ACCESS;
    }

    Inst inst = vm->program[ vm->registers[ REG_IP ].as_u64 ];

    switch ( inst.type )
    {
    case INST_NOP: vm->registers[ REG_IP ].as_u64++; break;

    case INST_PUSH:
        if ( vm->registers[ REG_ESP ].as_u64 >= AYM_STACK_SIZE )
        {
            return ERR_STACK_OVERFLOW;
        }
        vm->stack[ vm->registers[ REG_ESP ].as_u64++ ] = inst.dst.imm;
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_POP_STACK:
        if ( vm->registers[ REG_ESP ].as_u64 <= 0 )
        {
            return ERR_STACK_UNDERFLOW;
        }
        vm->registers[ REG_ESP ].as_u64--;
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_SWAP:
        if ( vm->registers[ REG_ESP ].as_u64 < 2 )
        {
            return ERR_STACK_UNDERFLOW;
        }
        i64 temp                                         = vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ].as_i64;
        vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ] = vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ];
        vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 = temp;
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_PLUS:
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

    case INST_SUB_STACK:
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

    case INST_DIV_STACK:
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

    case INST_MUL_STACK:
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

    case INST_DUP:
        if ( vm->registers[ REG_ESP ].as_u64 >= AYM_STACK_SIZE )
        {
            return ERR_STACK_OVERFLOW;
        }
        u64 addr_dup = aym_reslove_operand( vm, inst.dst ).as_u64;
        if ( vm->registers[ REG_ESP ].as_u64 - addr_dup <= 0 )
        {
            return ERR_STACK_UNDERFLOW;
        }
        vm->stack[ vm->registers[ REG_ESP ].as_u64 ] = vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 - addr_dup ];

        vm->registers[ REG_ESP ].as_u64++;
        vm->registers[ REG_IP ].as_u64++;
        break;

        // Registers Inst
    case INST_MOV:
        if ( inst.dst.type == OPERAND_REGISTER )
        {
            vm->registers[ inst.dst.reg ] = aym_reslove_operand( vm, inst.src );
        }
        else
        {
            /*throw err*/
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_POP_REG:

    case INST_ADD:
        if ( inst.dst.type == OPERAND_REGISTER )
        {
            vm->registers[ inst.dst.reg ].as_i64 += aym_reslove_operand( vm, inst.src ).as_i64;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_SUB_REG:

    case INST_DIV_REG:

    case INST_MUL_REG:

    case INST_XOR: {
        Word a = aym_reslove_operand( vm, inst.dst );
        Word b = aym_reslove_operand( vm, inst.src );

        vm->registers[ inst.dst.reg ].as_u64 = a.as_u64 ^ b.as_u64;
        vm->registers[ REG_FLAGS ].as_u64    = ( vm->registers[ inst.dst.reg ].as_u64 == 0 );
        break;
    }

    case INST_AND: {
        Word a = aym_reslove_operand( vm, inst.dst );
        Word b = aym_reslove_operand( vm, inst.src );

        vm->registers[ inst.dst.reg ].as_u64 = a.as_u64 && b.as_u64;
        vm->registers[ REG_FLAGS ].as_u64    = ( vm->registers[ inst.dst.reg ].as_u64 == 0 );
        break;
    }

    case INST_OR: {
        Word a = aym_reslove_operand( vm, inst.dst );
        Word b = aym_reslove_operand( vm, inst.src );

        vm->registers[ inst.dst.reg ].as_u64 = a.as_u64 || b.as_u64;
        vm->registers[ REG_FLAGS ].as_u64    = ( vm->registers[ inst.dst.reg ].as_u64 == 0 );
        break;
    }

    case INST_NOT: {
        Word a                               = aym_reslove_operand( vm, inst.src );
        vm->registers[ inst.dst.reg ].as_u64 = ~a.as_u64;
        vm->registers[ REG_FLAGS ].as_u64    = ( vm->registers[ inst.dst.reg ].as_u64 == 0 );
        break;
    }

    case INST_LEA: {
        Word addr                     = aym_reslove_operand( vm, inst.src );
        vm->registers[ inst.dst.reg ] = addr;
        break;
    }

    case INST_CMP: {
        Word r;
        r.as_i64 = aym_reslove_operand( vm, inst.src ).as_i64 - aym_reslove_operand( vm, inst.dst ).as_i64;

        vm->registers[ REG_FLAGS ].as_u64 = 0;
        if ( r.as_i64 == 0 )
        {
            vm->registers[ REG_FLAGS ].as_u64 |= FLAG_ZERO;
        }
        if ( r.as_i64 < 0 )
        {
            vm->registers[ REG_FLAGS ].as_u64 |= FLAG_SIGN;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;
    }

    case INST_TEST: {
        Word a = aym_reslove_operand( vm, inst.dst );
        Word b = aym_reslove_operand( vm, inst.src );
        Word res;

        res.as_u64                        = a.as_u64 & b.as_u64;
        vm->registers[ REG_FLAGS ].as_u64 = ( res.as_u64 == 0 );
        break;
    }
    case INST_LOAD: {
        Word addr                     = aym_reslove_operand( vm, inst.src );
        vm->registers[ inst.dst.reg ] = vm->memory[ addr.as_u64 ];
        break;
    }
    case INST_STORE: {
        Word addr                 = aym_reslove_operand( vm, inst.dst );
        Word value                = aym_reslove_operand( vm, inst.src );
        vm->memory[ addr.as_u64 ] = value;
        break;
    }

    // Control Flow
    case INST_JMP: vm->registers[ REG_IP ] = aym_reslove_operand( vm, inst.dst ); break;
    case INST_JNZ:
        if ( !( vm->registers[ REG_FLAGS ].as_u64 & FLAG_ZERO ) )
        {
            Word ip_to_jmp          = aym_reslove_operand( vm, inst.dst );
            vm->registers[ REG_IP ] = ip_to_jmp;
            break;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_JZ:
        if ( vm->registers[ REG_FLAGS ].as_u64 & FLAG_ZERO )
        {
            Word ip_to_jmp          = aym_reslove_operand( vm, inst.dst );
            vm->registers[ REG_IP ] = ip_to_jmp;
            break;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_CALL:
        if ( vm->registers[ REG_ESP ].as_u64 >= AYM_STACK_SIZE )
        {
            return ERR_STACK_OVERFLOW;
        }
        // push old BP to the Stack and set BP to ESP
        vm->stack[ vm->registers[ REG_ESP ].as_u64++ ] = vm->registers[ REG_BP ];
        vm->registers[ REG_BP ].as_u64                 = vm->registers[ REG_ESP ].as_u64 - 1;

        vm->stack[ vm->registers[ REG_ESP ].as_u64++ ] = vm->registers[ REG_IP ];
        vm->registers[ REG_IP ]                        = aym_reslove_operand( vm, inst.dst );
        break;

    case INST_RET:
        if ( vm->registers[ REG_ESP ].as_u64 < 1 )
        {
            return ERR_STACK_UNDERFLOW;
        }
        vm->registers[ REG_IP ].as_u64 = vm->stack[ vm->registers[ REG_BP ].as_u64 + 1 ].as_u64 + 1;
        vm->registers[ REG_BP ]        = vm->stack[ vm->registers[ REG_BP ].as_u64 ];
        // deallocating local varible
        vm->registers[ REG_ESP ]       = vm->registers[ REG_BP ];

        break;

    case INST_SYSCALL:
        if ( invoke_syscall( vm ) != ERR_OK )
        {
            return ERR_UNKOWN_SYSCALL;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;
    case INST_HALT: vm->halt = true; break;
    default       : printf( "ILLEGAL INST: %s, TYPE: %d\n", inst_as_cstr( inst.type ), inst.type ); return ERR_ILLEGAL_INST;
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

Word aym_reslove_operand( AYM *vm, Operand operand )
{
    if ( operand.type == OPERAND_IMMEDIATE )
    {
        return operand.imm;
    }
    else if ( operand.type == OPERAND_REGISTER )
    {
        return vm->registers[ operand.reg ];
    }
    else
    {
        return *( Word * )( vm->memory + operand.mem_addr );
    }
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
        "  Reg0: %p, Reg1: %p, Reg2: %p\n  Reg3: %p, Reg4: %p, Reg5: %p\n  Bp  : %p, Esp : %p, Ip  : %p\n  Flags: "
        "%p\n",
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
