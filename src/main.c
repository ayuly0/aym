#include "aym.h"
#include "make_inst.h"

int main( int argc, char **argv )
{
    static AYM vm = {};
    aym_init( &vm );

    /*char *str = "HELLO WORD\n";*/
    Inst pg[] = {
        // call 0x5
        /*MAKE_CALL( OPERAND_IMMEDIATE, imm, 0x5 ),*/
        MAKE_ADD( OPERAND_REGISTER, reg, REG_0, OPERAND_IMMEDIATE, imm, 1 ),
        MAKE_CMP( OPERAND_REGISTER, reg, REG_0, OPERAND_IMMEDIATE, imm, 5 ), MAKE_JNE( OPERAND_IMMEDIATE, imm, 0 ),
        MAKE_HALT,
        // write:
        /*MAKE_PUSH( OPERAND_IMMEDIATE, imm, 1 ),*/
        /*MAKE_PUSH( OPERAND_IMMEDIATE, imm, ( u64 )( uintptr_t )str ),*/
        /*MAKE_PUSH( OPERAND_IMMEDIATE, imm, 11 ),*/
        /*MAKE_PUSH( OPERAND_IMMEDIATE, imm, SYSCALL_WRITE ),*/
        /*MAKE_SYSCALL,*/
        /*MAKE_RET,*/
    };

    size_t prog_size = sizeof( pg ) / sizeof( pg[ 0 ] );
    size_t out_size  = 0;
    u8 *bytecode     = aym_inst_to_bytecode( pg, prog_size, &out_size );

    for ( int i = 0; i < out_size; ++i )
    {
        printf( "0x%02X ", bytecode[ i ] );
    }

    Inst *program_from_bytecode = aym_bytecode_to_inst( bytecode, out_size );
    aym_load_inst_from_mem( &vm, program_from_bytecode, prog_size );

    Err err = aym_execute_program( &vm );
    if ( err != ERR_OK )
    {
        return 1;
    }
    return 0;
}
