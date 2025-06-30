#include "aym.h"

#define MAKE_INST_PUSH( value ) { .type = INST_PUSH, .dst.imm = value }
#define MAKE_INST_PLUS          { .type = INST_PLUS }
#define MAKE_INST_SYSCALL       { .type = INST_SYSCALL }
#define MAKE_INST_HALT          { .type = INST_HALT }
#define MAKE_INST_MOV_REG_VAL( reg_, val_ )                                                                            \
    {                                                                                                                  \
        .type = INST_MOV, .dst = { .type = OPERAND_REGISTER, .reg = ( reg_ ) }, .src = {                               \
            .type = OPERAND_IMMEDIATE,                                                                                 \
            .imm  = ( val_ )                                                                                           \
        }                                                                                                              \
    }
#define MAKE_INST_MOV_REG_REG( reg_1, reg_2 )                                                                          \
    {                                                                                                                  \
        .type = INST_MOV, .dst = { .type = OPERAND_REGISTER, .reg = ( reg_1 ) }, .src = {                              \
            .type = OPERAND_REGISTER,                                                                                  \
            .reg  = ( reg_2 )                                                                                          \
        }                                                                                                              \
    }

int main( int argc, char **argv )
{
    static AYM vm = {};
    aym_init( &vm );

    char *str = "HELLO WORD\n";
    Inst pg[] = {
        /*
         * 0x0: mov r0, 1
         * 0x1: push 1
         * 0x2: push [str]
         * 0x3: push 11
         * 0x4: push SYSCALL_WRITE
         * 0x5: syscall
         * 0x6: add r0, 1
         * 0x7: cmp r0, 5
         * 0x8: jnz 0x1
         * 0x9: halt
         */

        MAKE_INST_MOV_REG_VAL( REG_0, 0 ),
        MAKE_INST_PUSH( 1 ),
        MAKE_INST_PUSH( ( u64 )( uintptr_t )str ),
        MAKE_INST_PUSH( 11 ),
        MAKE_INST_PUSH( SYSCALL_WRITE ),
        MAKE_INST_SYSCALL,
        ( Inst ){ INST_ADD, .dst = { OPERAND_REGISTER, .reg = REG_0 }, .src = { OPERAND_IMMEDIATE, .imm = 1 } },
        ( Inst ){ INST_CMP, { OPERAND_REGISTER, .reg = REG_0 }, { OPERAND_IMMEDIATE, .imm = 5 } },
        ( Inst ){ INST_JNZ, .dst = { OPERAND_IMMEDIATE, .imm = 1 } },
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
