#include "inst.h"

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

    case INST_JMP  : return "INST_JMP";
    case INST_JE   : return "INST_JE";
    case INST_JNE  : return "INST_JNE";
    case INST_JG   : return "INST_JG";
    case INST_JL   : return "INST_JL";
    case INST_JGE  : return "INST_JGE";
    case INST_JLE  : return "INST_JLE";
    case INST_JA   : return "INST_JA";
    case INST_JB   : return "INST_JB";
    case INST_JAE  : return "INST_JAE";
    case INST_JBE  : return "INST_JBE";
    case INST_INC  : return "INST_INC";
    case INST_CALL : return "INST_CALL";
    case INST_RET  : return "INST_RET";
    case INST_LABEL: return "INST_LABEL";

    case INST_SYSCALL: return "INST_SYSCALL";
    case INST_HALT   : return "INST_HALT";
    case INST_MOD    : return "INST_MOD";
    case INST_SHL    : return "INST_SHL";
    case INST_SHR    : return "INST_SHR";
    case INST_ROL    : return "INST_ROL";
    case INST_ROR    : return "INST_ROR";
    case INST_BSWAP  : return "INST_BSWAP";
    default          : return "UNKOWN_INST";
    }
}
