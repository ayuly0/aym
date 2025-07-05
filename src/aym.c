#include "aym.h"
#include "types.h"
#include <assert.h>
#include <stdlib.h>

char *err_as_cstr( Err err )
{
    switch ( err )
    {
    case ERR_STACK_OVERFLOW      : return "STACK_OVERFLOW";
    case ERR_STACK_UNDERFLOW     : return "STACK_UNDERFLOW";
    case ERR_ILLEGAL_INST_ACCESS : return "ERR_ILLEGAL_INST_ACCESS";
    case ERR_ILLEGAL_INST        : return "ERR_ILLEGAL_INST";
    case ERR_DIV_BY_ZERO         : return "ERR_DIV_BY_ZERO";
    case ERR_ILLEGAL_OPERAND_TYPE: return "ERR_ILLEGAL_OPERAND_TYPE";
    case ERR_UNKOWN_SYSCALL      : return "ERR_UNKNOWN_SYSCALL";
    default                      : return "UNKNOWN_ERR";
    }
}

char *aym_status_as_cstr( AYM_Status status )
{
    switch ( status )
    {
    case AYM_SUCCESS              : return "AYM_SUCCESS";
    case AYM_ERR_NULL_VM          : return "AYM_ERR_NULL_VM";
    case AYM_ERR_NULL_PROGRAM     : return "AYM_ERR_NULL_PROGRAM";
    case AYM_ERR_PROGRAM_TOO_LARGE: return "AYM_ERR_PROGRAM_TOO_LARGE";
    case AYM_ERR_INVALID_SIZE     : return "AYM_ERR_INVALID_SIZE";
    case AYM_ERR_MEMCPY_FAILED    : return "AYM_ERR_MEMCPY_FAILED";
    default                       : return "UNKNOWN_AYM_STATUS";
    }
}

char *inst_as_cstr( InstType inst_type )
{
    switch ( inst_type )
    {
    case INST_NOP : return "INST_NOP";
    case INST_PUSH: return "INST_PUSH";
    case INST_SWAP: return "INST_SWAP";
    case INST_PLUS: return "INST_PLUS";
    case INST_DUP : return "INST_DUP";

    case INST_SUB: return "INST_SUB";
    case INST_DIV: return "INST_DIV";
    case INST_MUL: return "INST_MUL";
    case INST_POP: return "INST_POP";

    case INST_MOV  : return "INST_MOV";
    case INST_ADD  : return "INST_ADD";
    case INST_XOR  : return "INST_XOR";
    case INST_AND  : return "INST_AND";
    case INST_NOT  : return "INST_NOT";
    case INST_OR   : return "INST_OR";
    case INST_LEA  : return "INST_LEA";
    case INST_CMP  : return "INST_CMP";
    case INST_TEST : return "INST_TEST";
    case INST_LOAD : return "INST_LOAD";
    case INST_STORE: return "INST_STORE";

    case INST_JMP : return "INST_JMP";
    case INST_JE  : return "INST_JE";
    case INST_JNE : return "INST_JNE";
    case INST_JG  : return "INST_JG";
    case INST_JL  : return "INST_JL";
    case INST_JGE : return "INST_JGE";
    case INST_JLE : return "INST_JLE";
    case INST_JA  : return "INST_JA";
    case INST_JB  : return "INST_JB";
    case INST_JAE : return "INST_JAE";
    case INST_JBE : return "INST_JBE";
    case INST_INC : return "INST_INC";
    case INST_CALL: return "INST_CALL";
    case INST_RET : return "INST_RET";

    case INST_SYSCALL: return "INST_SYSCALL";
    case INST_HALT   : return "INST_HALT";
    default          : return "UNKOWN_INST";
    }
}

u32 operand_as_u32( Operand operand )
{
    switch ( operand.type )
    {
    case OPERAND_IMMEDIATE: return operand.imm.as_u64;
    case OPERAND_REGISTER : return operand.reg;
    case OPERAND_MEMORY   : return operand.mem_addr;
    default               : return 0; // fallback for OPERAND_NONE or invalid
    }
}

Operand u32_as_operand( OperandType type, u32 value )
{
    Operand op = { .type = type };
    switch ( type )
    {
    case OPERAND_REGISTER : op.reg = value; break;
    case OPERAND_IMMEDIATE: op.imm.as_u64 = value; break;
    case OPERAND_MEMORY   : op.mem_addr = value; break;
    default               : break;
    }
    return op;
}

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

AYM_Status aym_load_inst_from_mem( AYM *vm, Inst *program, size_t program_size )
{
    if ( !vm )
    {
        return AYM_ERR_NULL_VM;
    }

    if ( !program )
    {
        return AYM_ERR_NULL_PROGRAM;
    }

    if ( program_size == 0 || program_size > AYM_MAX_PROGRAM_SIZE )
    {
        return AYM_ERR_PROGRAM_TOO_LARGE;
    }

    size_t total_bytes;
    if ( __builtin_mul_overflow( program_size, sizeof( Inst ), &total_bytes ) )
    {
        return AYM_ERR_INVALID_SIZE;
    }

    vm->program_size = program_size;
    memcpy( vm->program, program, sizeof( Inst ) * program_size );

    return AYM_SUCCESS;
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
