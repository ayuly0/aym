#ifndef AYM_LABEL_
#define AYM_LABEL_

#include "uthash.h"

#include "util.h"

typedef struct
{
    int id;
    char *name;
    u64 index;

    UT_hash_handle hh;
} LabelEntry;

void bind_label( LabelEntry **table, char *name, u64 index );

u64 get_label_index( LabelEntry **table, char *name );

void free_label_table( LabelEntry **table );

#endif // !AYM_LABEL_
