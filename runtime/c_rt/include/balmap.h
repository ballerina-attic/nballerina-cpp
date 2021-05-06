#ifndef __BALMAP__H__
#define __BALMAP__H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint64_t BalValue;

typedef struct {
    const char *value;
} BString;

typedef struct {
    BString key;
    BalValue value;
} Entry;

typedef struct {
    uint64_t header;
    Entry entries[];
} HashTable;

typedef struct {
    uint64_t header;
    size_t used;
    size_t capacity;
    HashTable *hashTable;
} BalMap;

BalMap *map_new(void);
void map_store(BalMap *ptr, BString *key, BalValue value);
bool map_load(BalMap *ptr, BString *key, BalValue *outValue);
void map_spread_field_init(BalMap *target, BalMap *src);

// Debugging only
void map_print(BalMap *ptr);

#endif //!__BALMAP__H__
