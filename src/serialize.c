#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "aym.h"
#include "serialize.h"

AYM_Status aym_load_inst_from_mem( AYM *vm, Inst *program, size_t program_size )
{
    if ( !vm )
    {
        return AYM_ERR_NULL_VM;
    }

    if ( !program )
    {
        return AYM_ERR_NULL_PROGRAM;
    }

    if ( program_size == 0 || program_size > AYM_MAX_PROGRAM_SIZE )
    {
        return AYM_ERR_PROGRAM_TOO_LARGE;
    }

    size_t total_bytes;
    if ( __builtin_mul_overflow( program_size, sizeof( Inst ), &total_bytes ) )
    {
        return AYM_ERR_INVALID_SIZE;
    }

    vm->program_size = program_size;
    memcpy( vm->program, program, sizeof( Inst ) * program_size );

    return AYM_OK;
}

AYM_Status aym_load_bytecode_from_file( AYM *vm, char *file_path )
{
    FILE *f = fopen( file_path, "rb" );
    if ( !f )
    {
        return AYM_ERR_FILE_NOT_FOUND;
    }

    uint32_t size = 0;
    if ( fread( &size, sizeof( uint32_t ), 1, f ) != 1 || size == 0 )
    {
        fclose( f );
        return AYM_ERR_INVALID_FORMAT;
    }

    u8 *buffer = malloc( size );
    if ( !buffer )
    {
        fclose( f );
        return AYM_ERR_ALLOC_FAILED;
    }

    size_t read_bytes = fread( buffer, 1, size, f );
    fclose( f );

    if ( read_bytes != size )
    {
        free( buffer );
        return AYM_ERR_IO;
    }

    Inst *program     = aym_bytecode_to_inst( buffer, size );
    AYM_Status status = aym_load_inst_from_mem( vm, program, size );

    free( buffer );
    return status;
}

AYM_Status aym_write_bytecode_to_file( u8 *bytecode, char *file_path, size_t bytecode_size )
{
    if ( !bytecode || !file_path )
    {
        return AYM_INVALID_ARGUMENT;
    }

    FILE *fp = fopen( file_path, "wb" );
    if ( !fp )
    {
        return AYM_FILE_ERROR;
    }

    size_t written = fwrite( bytecode, 1, bytecode_size, fp );
    fclose( fp );

    if ( written != bytecode_size )
    {
        return AYM_WRITE_ERROR;
    }

    return AYM_OK;
}

Inst *aym_bytecode_to_inst( u8 *bytecode, size_t bytecode_size )
{
    assert( bytecode || bytecode_size >= 2 && "bytecode is NULL or small than 2 bytes" );
    size_t ip       = 0;
    size_t capacity = 64;
    size_t count    = 0;
    Inst *program   = malloc( sizeof( Inst ) * capacity );

    assert( program && "program is NULL" );

    while ( ip < bytecode_size )
    {
        if ( count >= capacity )
        {
            capacity *= 2;
            program   = realloc( program, sizeof( Inst ) * capacity );
            if ( !program )
            {
                return NULL;
            }
        }

        u8 opcode   = bytecode[ ip++ ];
        u8 op_count = bytecode[ ip++ ];

        Operand dst = { .type = OPERAND_NONE };
        Operand src = { .type = OPERAND_NONE };

        if ( op_count >= 1 && ip + 1 < bytecode_size )
        {
            OperandType type = bytecode[ ip++ ];
            u8 val           = bytecode[ ip++ ];
            dst              = u8_as_operand( type, val );
        }

        if ( op_count == 2 && ip + 1 < bytecode_size )
        {
            OperandType type = bytecode[ ip++ ];
            u8 val           = bytecode[ ip++ ];
            src              = u8_as_operand( type, val );
        }

        program[ count++ ] = ( Inst ){ .type = ( InstType )opcode, .dst = dst, .src = src };
    }

    return program;
}

u8 *aym_inst_to_bytecode( Inst *program, size_t program_size, size_t *out_size )
{
    assert( program || program_size != 0 && "program is NULL" );

    size_t max_words = program_size * 6;
    u8 *bytecode     = malloc( sizeof( u8 ) * max_words );

    assert( bytecode && "bytecode is NULL" );
    if ( !bytecode )
    {
        return NULL;
    }

    size_t offset = 0;
    for ( size_t i = 0; i < program_size; ++i )
    {
        Inst *inst           = &program[ i ];
        bytecode[ offset++ ] = inst->type;
        printf( "Inst: %s (0x%02X)\n", inst_as_cstr( inst->type ), inst->type );

        int operand_count = 0;
        // operand 1 (dst)
        if ( inst->dst.type != OPERAND_NONE )
        {
            operand_count++;
        }

        // operand 2 (src)
        if ( inst->src.type != OPERAND_NONE )
        {
            operand_count++;
        }

        bytecode[ offset++ ] = operand_count;
        if ( inst->dst.type != OPERAND_NONE )
        {
            printf( "  Dst Operand: %d, Value: %d\n", inst->dst.type, operand_as_u8( inst->dst ) );
            bytecode[ offset++ ] = inst->dst.type;
            bytecode[ offset++ ] = operand_as_u8( inst->dst );
        }

        if ( inst->src.type != OPERAND_NONE )
        {
            printf( "  Src Operand: %d, Value: %d\n", inst->src.type, operand_as_u8( inst->src ) );
            bytecode[ offset++ ] = inst->src.type;
            bytecode[ offset++ ] = operand_as_u8( inst->src );
        }
    }

    if ( out_size )
    {
        *out_size = offset;
    }
    return bytecode;
}
