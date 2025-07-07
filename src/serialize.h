#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include "defs.h"
#include "inst.h"

typedef struct AYM_t AYM;

AYM_Status aym_load_inst_from_mem( AYM *vm, Inst *program, size_t program_size );

AYM_Status aym_load_bytecode_from_file( AYM *vm, char *file_path );

AYM_Status aym_write_bytecode_to_file( u8 *bytecode, char *file_path, size_t bytecode_size );

Inst *aym_bytecode_to_inst( u8 *bytecode, size_t bytecode_size );

u8 *aym_inst_to_bytecode( Inst *program, size_t program_size, size_t *out_size );

#endif // !SERIALIZE_H_
