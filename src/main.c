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

int main()
{
    static AYM vm = {};
    aym_init( &vm );

    Inst pg[] = {
        MAKE_INST_MOV_REG_VAL( REG_0, 69 ),
        MAKE_INST_MOV_REG_VAL( REG_1, 0 ),
        ( Inst ){ INST_ADD, { OPERAND_IMMEDIATE, .imm = 1 }, { OPERAND_REGISTER, .reg = REG_1 } },
        ( Inst ){ INST_CMP, { OPERAND_REGISTER, .reg = REG_0 }, { OPERAND_REGISTER, .reg = REG_1 } },
        ( Inst ){ INST_JNZ, .dst = { OPERAND_IMMEDIATE, .imm = 2 } },
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
