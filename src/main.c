#include "aym.h"
#include "defs.h"
#include "label.h"
#include "make_inst.h"
#include "serialize.h"
#include "syscall.h"

int main( int argc, char **argv )
{
    static AYM vm = {};
    aym_init( &vm );

    char *str = "HELLO WORD\n";
    Inst pg[] = {
        // call 0x5
        MAKE_LABEL( "loop" ),
        MAKE_CALL( OPERAND_LABEL, label_name, "write" ),
        MAKE_ADD( OPERAND_REGISTER, reg, REG_4, OPERAND_IMMEDIATE, imm, 1 ),
        MAKE_CMP( OPERAND_REGISTER, reg, REG_4, OPERAND_IMMEDIATE, imm, 5 ),
        MAKE_JNE( OPERAND_LABEL, label_name, "loop" ),
        MAKE_HALT,
        // write:
        MAKE_LABEL( "write" ),
        MAKE_MOV( OPERAND_REGISTER, reg, REG_0, OPERAND_IMMEDIATE, imm, SYSCALL_WRITE ),
        MAKE_MOV( OPERAND_REGISTER, reg, REG_1, OPERAND_IMMEDIATE, imm, 1 ),
        MAKE_MOV( OPERAND_REGISTER, reg, REG_2, OPERAND_IMMEDIATE, imm, ( u64 )( uintptr_t )str ),
        MAKE_MOV( OPERAND_REGISTER, reg, REG_3, OPERAND_IMMEDIATE, imm, 11 ),
        MAKE_SYSCALL,
        MAKE_RET,
    };

    size_t prog_size = sizeof( pg ) / sizeof( pg[ 0 ] );
    // size_t out_size  = 0;
    // u8 *bytecode     = aym_inst_to_bytecode( pg, prog_size, &out_size );
    //
    // for ( int i = 0; i < out_size; ++i )
    // {
    //     printf( "0x%02X ", bytecode[ i ] );
    // }
    //
    // aym_write_bytecode_to_file( bytecode, "loop.aym", out_size );
    // aym_load_bytecode_from_file( &vm, "loop.aym", bytecode, &out_size );

    // Inst *program_from_bytecode = aym_bytecode_to_inst( bytecode, out_size );
    // aym_load_inst_from_mem( &vm, program_from_bytecode, prog_size );
    aym_load_inst_from_mem( &vm, pg, prog_size );

    aym_bind_label( &vm );

    Err err = aym_execute_program( &vm );
    if ( err != ERR_OK )
    {
        return 1;
    }
    return 0;
}
