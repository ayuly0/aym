#include "aym.h"

#define MAKE_INST_PUSH( value ) { .type = INST_PUSH, .operand = value }
#define MAKE_INST_PLUS          { .type = INST_PLUS }
#define MAKE_INST_SYSCALL       { .type = INST_SYSCALL }
#define MAKE_INST_HALT          { .type = INST_HALT }

int main()
{
    static AYM vm = { .stack_size = 0, .program_size = 0, .ip = 0, .halt = false };

    char *str = "HELLO WORD\n";
    Inst pg[] = {
        MAKE_INST_PUSH( 35 ),
        MAKE_INST_PUSH( 34 ),
        MAKE_INST_PLUS,
        MAKE_INST_PUSH( 1 ),
        MAKE_INST_PUSH( ( u64 )( uintptr_t )str ),
        MAKE_INST_PUSH( 11 ),
        MAKE_INST_PUSH( SYSCALL_WRITE ),
        MAKE_INST_SYSCALL,
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
