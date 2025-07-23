#ifndef UTIL_H_
#define UTIL_H_

#include "defs.h"
#include "operand.h"

char *err_as_cstr( Err err );

char *aym_status_as_cstr( AYM_Status status );

char *operand_type_as_cstr( OperandType op_type );

#endif // !UTIL_H_
