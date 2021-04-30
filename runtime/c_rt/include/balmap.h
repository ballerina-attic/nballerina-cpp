#ifndef __BALMAP__H__
#define __BALMAP__H__

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t BalValue;

struct BString {
    char *value;
};

struct Entry {
    struct BString key;
    BalValue value;
};

struct HashTable {
    uint64_t header;
    struct Entry entries[];
};

struct BalMap {
    uint64_t header;
    uint64_t used;
    uint64_t capacity;
    struct HashTable *hashTable;
};

struct BalMap *map_new(void);
void map_deint(struct BalMap *ptr);
void map_store(struct BalMap *ptr, struct BString *key, BalValue value);
bool map_load(struct BalMap *ptr, struct BString *key, BalValue *outValue);
void map_spread_field_init(struct BalMap *target, struct BalMap *src);

// Debugging only
void map_print(struct BalMap *ptr);

#endif //!__BALMAP__H__
