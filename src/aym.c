#include "aym.h"
#include <assert.h>

void aym_init( AYM *vm )
{
    vm->halt = false;

    memset( vm->stack, 0, AYM_MAX_STACK_SIZE );
    memset( vm->memory, 0, AYM_MAX_MEMORY_SIZE );
    memset( vm->program, 0, AYM_MAX_PROGRAM_SIZE );
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
        if ( inst.dst.reg != 0 )
        {
            if ( vm->registers[ REG_ESP ].as_u64 < 1 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            vm->stack[ vm->registers[ REG_ESP ].as_u64++ ] = aym_reslove_operand( vm, inst.dst );
        }
        else
        {
            if ( vm->registers[ REG_ESP ].as_u64 >= AYM_MAX_STACK_SIZE )
            {
                return ERR_STACK_OVERFLOW;
            }
            vm->stack[ vm->registers[ REG_ESP ].as_u64++ ] = aym_reslove_operand( vm, inst.dst );
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_POP:
        if ( inst.dst.reg != 0 )
        {
            if ( vm->registers[ REG_ESP ].as_u64 < 1 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            vm->registers[ inst.dst.reg ].as_u64 = vm->stack[ --vm->registers[ REG_ESP ].as_u64 ].as_u64;
        }
        {
            if ( vm->registers[ REG_ESP ].as_u64 <= 0 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            vm->registers[ REG_ESP ].as_u64--;
        }
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

    case INST_SUB:
        if ( inst.dst.reg != 0 )
        {
            if ( inst.dst.type == OPERAND_REGISTER )
            {
                vm->registers[ inst.dst.reg ].as_i64 -= aym_reslove_operand( vm, inst.src ).as_i64;
            }
            else
            {
                return ERR_ILLEGAL_OPERAND_TYPE;
            }
        }
        else
        {
            if ( vm->registers[ REG_ESP ].as_u64 < 2 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 =
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 -
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ].as_i64;
            vm->registers[ REG_ESP ].as_u64--;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_DIV:

        if ( inst.dst.reg != 0 )
        {
            if ( inst.dst.type == OPERAND_REGISTER )
            {
                i64 value = aym_reslove_operand( vm, inst.src ).as_i64;
                if ( value == 0 )
                {
                    return ERR_DIV_BY_ZERO;
                }

                vm->registers[ inst.dst.reg ].as_i64 /= value;
            }
            else
            {
                return ERR_ILLEGAL_OPERAND_TYPE;
            }
        }
        else
        {
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
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_MUL:
        if ( inst.dst.reg != 0 )
        {
            if ( inst.dst.type == OPERAND_REGISTER )
            {
                vm->registers[ inst.dst.reg ].as_i64 *= aym_reslove_operand( vm, inst.src ).as_i64;
            }
            else
            {
                return ERR_ILLEGAL_OPERAND_TYPE;
            }
        }
        else
        {
            if ( vm->registers[ REG_ESP ].as_u64 < 2 )
            {
                return ERR_STACK_UNDERFLOW;
            }
            vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 =
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 2 ].as_i64 *
                vm->stack[ vm->registers[ REG_ESP ].as_u64 - 1 ].as_i64;
            vm->registers[ REG_ESP ].as_u64--;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_DUP:
        if ( vm->registers[ REG_ESP ].as_u64 >= AYM_MAX_STACK_SIZE )
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
            return ERR_ILLEGAL_OPERAND_TYPE;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_ADD:
        if ( inst.dst.type == OPERAND_REGISTER )
        {
            vm->registers[ inst.dst.reg ].as_i64 += aym_reslove_operand( vm, inst.src ).as_i64;
        }
        else
        {
            return ERR_ILLEGAL_OPERAND_TYPE;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

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
        Word a = aym_reslove_operand( vm, inst.src );
        Word b = aym_reslove_operand( vm, inst.dst );
        Word r;
        r.as_i64 = a.as_i64 - b.as_i64;

        vm->registers[ REG_FLAGS ].as_u64 = 0;
        if ( r.as_i64 == 0 )
        {
            vm->registers[ REG_FLAGS ].as_u64 |= FLAG_ZERO;
        }
        if ( r.as_i64 < 0 )
        {
            vm->registers[ REG_FLAGS ].as_u64 |= FLAG_SIGN;
        }
        if ( ( ( a.as_i64 ^ b.as_i64 ) & ( a.as_i64 ^ r.as_i64 ) ) < 0 )
        {
            vm->registers[ REG_FLAGS ].as_u64 |= FLAG_OVERFLOW;
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
    case INST_JNE:
        if ( !( vm->registers[ REG_FLAGS ].as_u64 & FLAG_ZERO ) )
        {
            Word ip_to_jmp          = aym_reslove_operand( vm, inst.dst );
            vm->registers[ REG_IP ] = ip_to_jmp;
            break;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_JE:
        if ( vm->registers[ REG_FLAGS ].as_u64 & FLAG_ZERO )
        {
            Word ip_to_jmp          = aym_reslove_operand( vm, inst.dst );
            vm->registers[ REG_IP ] = ip_to_jmp;
            break;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;

    case INST_JG: {
        Word flags = vm->registers[ REG_FLAGS ];
        if ( !( flags.as_u64 & FLAG_ZERO ) && ( ( flags.as_u64 & FLAG_SIGN ) == ( flags.as_u64 & FLAG_OVERFLOW ) ) )
        {
            vm->registers[ REG_IP ] = aym_reslove_operand( vm, inst.dst );
        }
        break;
    }

    case INST_JL: {
        Word flags = vm->registers[ REG_FLAGS ];
        if ( ( flags.as_u64 & FLAG_SIGN ) != ( flags.as_u64 & FLAG_OVERFLOW ) )
        {
            vm->registers[ REG_IP ] = aym_reslove_operand( vm, inst.dst );
        }
        break;
    }

    case INST_JGE: {
        Word flags = vm->registers[ REG_FLAGS ];
        if ( ( flags.as_u64 & FLAG_SIGN ) == ( flags.as_u64 & FLAG_OVERFLOW ) )
        {
            vm->registers[ REG_IP ] = aym_reslove_operand( vm, inst.dst );
        }
        break;
    }

    case INST_JLE: {
        Word flags = vm->registers[ REG_FLAGS ];
        if ( ( flags.as_u64 & FLAG_ZERO ) || ( ( flags.as_u64 & FLAG_SIGN ) != ( flags.as_u64 & FLAG_OVERFLOW ) ) )
        {
            vm->registers[ REG_IP ] = aym_reslove_operand( vm, inst.dst );
        }
        break;
    }

    case INST_JA: {
        Word flags = vm->registers[ REG_FLAGS ];
        if ( !( flags.as_u64 & FLAG_ZERO ) && !( flags.as_u64 & FLAG_CARRY ) )
        {
            vm->registers[ REG_IP ] = aym_reslove_operand( vm, inst.dst );
        }
        break;
    }

    case INST_JB: {
        Word flags = vm->registers[ REG_FLAGS ];
        if ( flags.as_u64 & FLAG_CARRY )
        {
            vm->registers[ REG_IP ] = aym_reslove_operand( vm, inst.dst );
        }
        break;
    }

    case INST_JAE: {
        Word flags = vm->registers[ REG_FLAGS ];
        if ( !( flags.as_u64 & FLAG_CARRY ) )
        {
            vm->registers[ REG_IP ] = aym_reslove_operand( vm, inst.dst );
        }
        break;
    }

    case INST_JBE: {
        Word flags = vm->registers[ REG_FLAGS ];
        if ( ( flags.as_u64 & FLAG_ZERO ) && ( flags.as_u64 & FLAG_CARRY ) )
        {
            vm->registers[ REG_IP ] = aym_reslove_operand( vm, inst.dst );
        }
        break;
    }

    case INST_INC: {
        if ( inst.dst.type == OPERAND_REGISTER )
        {
            vm->registers[ inst.dst.reg ].as_u64++;
            vm->registers[ REG_FLAGS ].as_u64 = ( vm->registers[ inst.dst.reg ].as_u64 == 0 );
        }
        else
        {
            return ERR_ILLEGAL_OPERAND_TYPE;
        }
        vm->registers[ REG_IP ].as_u64++;
        break;
    }

    case INST_CALL:
        if ( vm->registers[ REG_ESP ].as_u64 >= AYM_MAX_STACK_SIZE )
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
        "  Reg0: %p, Reg1: %p, Reg2: %p\n  Reg3: %p, Reg4: %p, Reg5: %p\n  Reg6: %p, Reg7: %p, Reg8: %p\n  Reg9: %p, "
        "Bp  : %p, "
        "Esp : %p\n  Ip  : %p,  Flags: "
        "%p\n",
        vm->registers[ REG_0 ].as_ptr, vm->registers[ REG_1 ].as_ptr, vm->registers[ REG_2 ].as_ptr,
        vm->registers[ REG_3 ].as_ptr, vm->registers[ REG_4 ].as_ptr, vm->registers[ REG_5 ].as_ptr,
        vm->registers[ REG_6 ].as_ptr, vm->registers[ REG_7 ].as_ptr, vm->registers[ REG_8 ].as_ptr,
        vm->registers[ REG_9 ].as_ptr, vm->registers[ REG_BP ].as_ptr, vm->registers[ REG_ESP ].as_ptr,
        vm->registers[ REG_IP ].as_ptr, vm->registers[ REG_FLAGS ].as_ptr
    );
}
