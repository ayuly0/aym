#ifndef TPYES_H
#define TPYES_H

#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int8_t i8;

typedef union {
    u64 as_u64;
    i64 as_i64;
    double as_f64;
    void *as_ptr;
} Word;

#endif // !TPYES_H
