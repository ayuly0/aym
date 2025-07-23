#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "aym.h"
#include "defs.h"
#include "label.h"
#include "serialize.h"

u32 compute_checksum( const u8 *data, size_t len )
{
    u32 sum = 0;
    for ( size_t i = 0; i < len; i++ )
    {
        sum += data[ i ];
    }
    return sum;
}

void aym_bind_labels( Inst *program, size_t program_size, LabelEntry **label_table )
{
    for ( u64 i = 0; i < program_size; ++i )
    {
        Inst inst = program[ i ];
        if ( inst.type == INST_LABEL )
        {
            bind_label( label_table, inst.dst.label_name, i );
            printf( "Bind Label: name: %s, addr: 0x%02X\n", inst.dst.label_name, i );
        }
    }
}

void aym_resolve_labels( Inst *program, size_t program_size, LabelEntry **label_table )
{
    for ( size_t i = 0; i < program_size; ++i )
    {
        Inst *inst = &program[ i ];
        if ( inst->dst.type == OPERAND_LABEL && inst->dst.label_name )
        {
            u64 index = get_label_index( label_table, inst->dst.label_name );
            if ( index != ( u64 )-1 )
            {
                inst->dst.type       = OPERAND_IMMEDIATE;
                inst->dst.imm        = ( Word ){ .as_u64 = index };
                inst->dst.label_name = NULL;
            }
            else
            {
                fprintf( stderr, "Error: unresolved label '%s' in instruction 0x%02X\n", inst->dst.label_name, i );
                _exit( 1 );
            }
        }
    }
}

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

AYM_Status aym_load_bytecode_from_file( AYM *vm, char *file_path, u8 *out_bytecode, size_t *out_size )
{
    if ( !file_path || !out_bytecode || !out_size )
    {
        return AYM_INVALID_ARGUMENT;
    }

    FILE *fp = fopen( file_path, "rb" );
    if ( !fp )
    {
        return AYM_FILE_ERROR;
    }

    // Read header
    AYM_Header hdr;
    if ( fread( &hdr, 1, sizeof( hdr ), fp ) != sizeof( hdr ) )
    {
        fclose( fp );
        return AYM_READ_ERROR;
    }

    // Validate header
    if ( hdr.magic != AYM_MAGIC || hdr.version != AYM_VERSION || hdr.header_size != sizeof( AYM_Header ) )
    {
        fclose( fp );
        return AYM_BAD_HEADER;
    }

    // Allocate buffer for bytecode
    u8 *buf = ( u8 * )malloc( hdr.bytecode_size );
    if ( !buf )
    {
        fclose( fp );
        return AYM_FILE_ERROR;
    }

    // Read bytecode blob
    if ( fread( buf, 1, hdr.bytecode_size, fp ) != hdr.bytecode_size )
    {
        free( buf );
        fclose( fp );
        return AYM_READ_ERROR;
    }
    fclose( fp );

    // Verify checksum
    uint32_t chk = compute_checksum( buf, hdr.bytecode_size );
    if ( chk != hdr.checksum )
    {
        free( buf );
        return AYM_CHECKSUM_MISMATCH;
    }

    out_bytecode = buf;
    *out_size    = hdr.bytecode_size;
    return AYM_OK;
}

AYM_Status aym_write_bytecode_to_file( u8 *bytecode, char *file_path, size_t bytecode_size )
{
    if ( !bytecode || !file_path || bytecode_size == 0 )
    {
        return AYM_INVALID_ARGUMENT;
    }

    FILE *fp = fopen( file_path, "wb" );
    if ( !fp )
    {
        return AYM_FILE_ERROR;
    }

    // Fill header
    AYM_Header hdr;
    hdr.magic         = AYM_MAGIC;
    hdr.version       = AYM_VERSION;
    hdr.header_size   = sizeof( AYM_Header );
    hdr.bytecode_size = ( uint32_t )bytecode_size;
    hdr.checksum      = compute_checksum( bytecode, bytecode_size );

    // Write header
    if ( fwrite( &hdr, 1, sizeof( hdr ), fp ) != sizeof( hdr ) )
    {
        fclose( fp );
        return AYM_WRITE_ERROR;
    }

    // Write bytecode blob
    if ( fwrite( bytecode, 1, bytecode_size, fp ) != bytecode_size )
    {
        fclose( fp );
        return AYM_WRITE_ERROR;
    }

    fclose( fp );
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

    size_t max_words        = program_size * 6;
    u8 *bytecode            = malloc( sizeof( u8 ) * max_words );
    LabelEntry *label_table = NULL;

    assert( bytecode && "bytecode is NULL" );
    if ( !bytecode )
    {
        return NULL;
    }

    aym_bind_labels( program, program_size, &label_table );
    // aym_resolve_labels( program, program_size, &label_table );

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
            if ( inst->dst.type == OPERAND_LABEL && inst->dst.label_name )
            {
                u64 index = get_label_index( &label_table, inst->dst.label_name );
                if ( index != ( u64 )-1 )
                {
                    inst->dst.type       = OPERAND_IMMEDIATE;
                    inst->dst.imm.as_u64 = index;
                }
                else
                {
                    fprintf( stderr, "Error: unresolved label '%s' in instruction 0x%02X\n", inst->dst.label_name, i );
                    _exit( 1 );
                }
            }

            bytecode[ offset++ ] = inst->dst.type;
            bytecode[ offset++ ] = operand_as_u8( inst->dst );

            printf(
                "  Dst Operand: %s, Value: %d\n", operand_type_as_cstr( inst->dst.type ), operand_as_u8( inst->dst )
            );
        }

        if ( inst->src.type != OPERAND_NONE )
        {
            bytecode[ offset++ ] = inst->src.type;
            bytecode[ offset++ ] = operand_as_u8( inst->src );

            printf(
                "  Src Operand: %s, Value: %d\n", operand_type_as_cstr( inst->src.type ), operand_as_u8( inst->src )
            );
        }
    }

    if ( out_size )
    {
        *out_size = offset;
    }
    return bytecode;
}
