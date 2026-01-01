#ifndef SC6_ALLOC_H
#define SC6_ALLOC_H

#include <stdatomic.h>

struct c_countChanges {
    atomic_ullong countMalloc;
    atomic_ullong countCalloc;
    atomic_ullong countFree;
};

void c_initSem();

[[nodiscard]] void *c_malloc(int size);

[[nodiscard]] void *c_calloc(int count, int size);

void c_free(void *pointer);

[[nodiscard]] void *c_realloc(void *pointer, size_t size);

[[nodiscard]] struct c_countChanges *c_result();

#endif //SC6_ALLOC_H
