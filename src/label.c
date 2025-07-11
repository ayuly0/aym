#include <stdio.h>
#include <stdlib.h>

#include "label.h"

void bind_label( LabelEntry **table, char *name, u64 index )
{
    LabelEntry *entry = malloc( sizeof( LabelEntry ) );
    entry->name       = strdup( name );
    entry->index      = index;
    HASH_ADD_KEYPTR( hh, *table, entry->name, strlen( entry->name ), entry );
}

u64 get_label_index( LabelEntry **table, char *name )
{
    LabelEntry *entry = NULL;
    HASH_FIND_STR( *table, name, entry );
    if ( !entry )
    {
        fprintf( stderr, "Error: label '%s' not found\n", name );
        return -1;
    }
    return entry->index;
}

void free_label_table( LabelEntry **table )
{
    LabelEntry *cur, *tmp;
    HASH_ITER( hh, *table, cur, tmp )
    {
        HASH_DEL( *table, cur );
        free( ( char * )cur->name ); // strdup'd string
        free( cur );
    }
}
