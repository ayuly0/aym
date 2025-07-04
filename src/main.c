#include "aym.h"
#include "make_inst.h"

int main( int argc, char **argv )
{
    static AYM vm = {};
    aym_init( &vm );

    char *str = "HELLO WORD\n";
    Inst pg[] = {
        // call 0x5
        MAKE_CALL( OPERAND_IMMEDIATE, imm, 0x5 ),
        MAKE_ADD( OPERAND_REGISTER, reg, REG_0, OPERAND_IMMEDIATE, imm, 1 ),
        MAKE_CMP( OPERAND_REGISTER, reg, REG_0, OPERAND_IMMEDIATE, imm, 5 ),
        MAKE_JNE( OPERAND_IMMEDIATE, imm, 0 ),
        MAKE_HALT,
        // write:
        MAKE_PUSH( OPERAND_IMMEDIATE, imm, 1 ),
        MAKE_PUSH( OPERAND_IMMEDIATE, imm, ( u64 )( uintptr_t )str ),
        MAKE_PUSH( OPERAND_IMMEDIATE, imm, 11 ),
        MAKE_PUSH( OPERAND_IMMEDIATE, imm, SYSCALL_WRITE ),
        MAKE_SYSCALL,
        MAKE_RET,
    };

    aym_load_program_from_mem( &vm, pg, sizeof( pg ) / sizeof( pg[ 0 ] ) );

    Err err = aym_execute_program( &vm );
    if ( err != ERR_OK )
    {
        return 1;
    }
    return 0;
}
