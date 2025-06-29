#include "aym.h"

#define MAKE_INST_PUSH( value ) { .type = INST_STACK, .opcode = INST_STACK_PUSH, .src = value }
#define MAKE_INST_PLUS          { .type = INST_STACK, .opcode = INST_STACK_PLUS }
#define MAKE_INST_SYSCALL       { .type = INST_STACK, .opcode = INST_STACK_SYSCALL }
#define MAKE_INST_HALT          { .type = INST_STACK, .opcode = INST_STACK_HALT }
#define MAKE_INST_MOV_REG_VAL( reg_, val_ )                                                                            \
    ( Inst )                                                                                                           \
    {                                                                                                                  \
        .type = INST_REG, .opcode = INST_REG_MOV, .dst = { .type = OPERAND_REGISTER, .reg = ( reg_ ) }, .src = {       \
            .type = OPERAND_IMMEDIATE,                                                                                 \
            .imm  = ( val_ )                                                                                           \
        }                                                                                                              \
    }

int main()
{
    static AYM vm = {};
    aym_init( &vm );

    char *str = "HELLO WORD\n";
    Inst pg[] = {
        MAKE_INST_PUSH( 35 ),
        MAKE_INST_PUSH( 34 ),
        MAKE_INST_PLUS,
        MAKE_INST_PUSH( 1 ),
        MAKE_INST_PUSH( ( u64 )( uintptr_t )str ),
        MAKE_INST_PUSH( 11 ),
        MAKE_INST_PUSH( ( int )SYSCALL_WRITE ),
        /*MAKE_INST_SYSCALL,*/
        MAKE_INST_MOV_REG_VAL( REG_0, 69 ),
        MAKE_INST_HALT,
    };
    vm.program_size = sizeof( pg ) / sizeof( pg[ 0 ] );
    memcpy( vm.program, pg, sizeof( pg[ 0 ] ) * vm.program_size );

    Err err = aym_execute_program( &vm );
    if ( err != ERR_OK )
    {
        return 1;
    }
    return 0;
}
