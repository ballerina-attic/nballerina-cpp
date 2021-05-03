#include "include/balmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const size_t MULTI_FACTOR = 2;
static const uint64_t INITIAL_SIZE = 2;
static const uint64_t HEADER_MAP = 42;
static const uint64_t HEADER_HASH_TABLE = 42;

// Placeholder for GC allocator
static void *gc_alloc(size_t size) {
    void *retVal = malloc(size);
    if (!retVal) {
        printf("Memory allocation failed!\n");
        abort();
    }
    return retVal;
}

// Placeholder in for GC re-allocator
static void *gc_realloc(void *ptr, size_t size) {
    void *retVal = realloc(ptr, size);
    if (!retVal) {
        printf("Memory allocation failed!\n");
        abort();
    }
    return retVal;
}

// Placeholder in for BString compare
static bool compareBString(struct BString *str1, struct BString *str2) { return strcmp(str1->value, str2->value) == 0; }

// Placeholder in for BString clone
static struct BString *cloneBString(struct BString *src) {
    struct BString *clone = gc_alloc(sizeof(struct BString));
    clone->value = gc_alloc(strlen(src->value) + 1);
    strcpy(clone->value, src->value);
    return clone;
}

static size_t getSizeOfHashTable(uint64_t nEntries) {
    return sizeof(struct HashTable) + (sizeof(struct Entry) * nEntries);
}

struct BalMap *map_new(void) {
    struct BalMap *retVal = gc_alloc(sizeof(struct BalMap));
    retVal->header = HEADER_MAP;
    retVal->used = 0;
    retVal->capacity = INITIAL_SIZE;
    retVal->hashTable = gc_alloc(getSizeOfHashTable(INITIAL_SIZE));
    retVal->hashTable->header = HEADER_HASH_TABLE;
    return retVal;
}

void map_deint(struct BalMap *ptr) {
    free(ptr->hashTable);
    free(ptr);
}

void map_store(struct BalMap *ptr, struct BString *key, BalValue value) {
    size_t currentUsed = ptr->used;
    if (currentUsed == ptr->capacity) {
        size_t newCapacity = ptr->capacity * MULTI_FACTOR;
        ptr->hashTable = gc_realloc(ptr->hashTable, getSizeOfHashTable(newCapacity));
        ptr->capacity = newCapacity;
    }
    ptr->hashTable->entries[currentUsed].key = *cloneBString(key);
    ptr->hashTable->entries[currentUsed].value = value;
    ptr->used++;
}

bool map_load(struct BalMap *ptr, struct BString *key, BalValue *outValue) {
    bool retVal = false;
    uint64_t used = ptr->used;
    for (size_t i = 0; i < used; ++i) {
        if (compareBString(&ptr->hashTable->entries[i].key, key)) {
            retVal = true;
            *outValue = ptr->hashTable->entries[i].value;
            break;
        }
    }
    return retVal;
}

void map_spread_field_init(struct BalMap *target, struct BalMap *src) {

    uint64_t srcUsed = src->used;
    uint64_t targetUsed = target->used;
    uint64_t newUsed = srcUsed + targetUsed;

    if (newUsed > target->capacity) {
        size_t newCapacity = target->capacity * MULTI_FACTOR;
        while (newUsed > newCapacity) {
            newCapacity *= MULTI_FACTOR;
        }
        target->hashTable = gc_realloc(target->hashTable, getSizeOfHashTable(newCapacity));
        target->capacity = newCapacity;
    }

    for (size_t i = 0; i < srcUsed; i++) {
        target->hashTable->entries[targetUsed + i].value = src->hashTable->entries[i].value;
        target->hashTable->entries[targetUsed + i].key = *cloneBString(&src->hashTable->entries[i].key);
    }
    target->used = newUsed;
}

void map_print(struct BalMap *ptr) {
    printf("-------------\n");
    if (!ptr) {
        printf("Invalid BalMap ptr\n");
        abort();
    }
    printf("Map size: %zu\n", ptr->used);
    for (size_t i = 0; i < ptr->used; i++) {
        printf("\t key: %s", ptr->hashTable->entries[i].key.value);
        printf("\t\t value: %zu\n", (uint64_t)ptr->hashTable->entries[i].value);
    }
    printf("-------------\n");
}
