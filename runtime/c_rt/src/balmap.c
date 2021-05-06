#include "include/balmap.h"
#include <stdio.h>
#include <string.h>

static const size_t INITIAL_SIZE = 2;
static const uint64_t HEADER_MAP = 42;
static const uint64_t HEADER_HASH_TABLE = 42;

// Placeholder for GC allocator
static void *gc_alloc(size_t size) {
    void *retVal = malloc(size);
    return retVal;
}

// Placeholder in for GC re-allocator
static void *gc_realloc(void *ptr, size_t size) {
    void *retVal = realloc(ptr, size);
    return retVal;
}

// Placeholder in for BString compare
static bool compareBString(BString *str1, BString *str2) { return strcmp(str1->value, str2->value) == 0; }

static size_t getSizeOfHashTable(size_t nEntries) {
    size_t structSize = sizeof(HashTable);
    size_t entriesSize = sizeof(Entry) * nEntries;
    if (entriesSize > (SIZE_MAX - structSize)) {
        fprintf(stderr, "%s", "Out of memory!\n");
        abort(); // abort if size_t overflows
    }
    return structSize + entriesSize;
}

static size_t newGrowthSize(size_t currentCapacity) {
    if (currentCapacity == SIZE_MAX) {
        fprintf(stderr, "%s", "Out of memory!\n");
        abort(); // abort if capacity is max
    }
    if (currentCapacity > (SIZE_MAX >> 1)) {
        return SIZE_MAX;
    }
    return currentCapacity << 1;
}

BalMap *map_new(void) {
    BalMap *retVal = gc_alloc(sizeof(BalMap));
    if (!retVal) {
        fprintf(stderr, "%s", "Memory alloc failed!\n");
        abort(); // abort if alloc failed
    }
    retVal->header = HEADER_MAP;
    retVal->used = 0;
    retVal->capacity = INITIAL_SIZE;
    retVal->hashTable = gc_alloc(getSizeOfHashTable(INITIAL_SIZE));
    if (!retVal->hashTable) {
        fprintf(stderr, "%s", "Memory alloc failed!\n");
        abort(); // abort if alloc failed
    }
    retVal->hashTable->header = HEADER_HASH_TABLE;
    return retVal;
}

void map_store(BalMap *ptr, BString *key, BalValue value) {
    size_t currentUsed = ptr->used;
    if (currentUsed == ptr->capacity) {
        size_t newCapacity = newGrowthSize(ptr->capacity);
        ptr->hashTable = gc_realloc(ptr->hashTable, getSizeOfHashTable(newCapacity));
        if (!ptr->hashTable) {
            fprintf(stderr, "%s", "Memory alloc failed!\n");
            abort(); // abort if alloc failed
        }
        ptr->capacity = newCapacity;
    }
    ptr->hashTable->entries[currentUsed].key = *key;
    ptr->hashTable->entries[currentUsed].value = value;
    ptr->used++;
}

bool map_load(BalMap *ptr, BString *key, BalValue *outValue) {
    bool retVal = false;
    size_t used = ptr->used;
    for (size_t i = 0; i < used; ++i) {
        if (compareBString(&ptr->hashTable->entries[i].key, key)) {
            retVal = true;
            *outValue = ptr->hashTable->entries[i].value;
            break;
        }
    }
    return retVal;
}

void map_spread_field_init(BalMap *target, BalMap *src) {

    size_t srcUsed = src->used;
    size_t targetUsed = target->used;
    if (srcUsed > (SIZE_MAX - targetUsed)) {
        fprintf(stderr, "%s", "Out of memory!\n");
        abort(); // abort if size_t overflows
    }

    size_t newUsed = srcUsed + targetUsed;
    if (newUsed > target->capacity) {
        size_t newCapacity = target->capacity;
        while (newUsed > newCapacity) {
            newCapacity <<= 1;
        }
        target->hashTable = gc_realloc(target->hashTable, getSizeOfHashTable(newCapacity));
        target->capacity = newCapacity;
    }

    for (size_t i = 0; i < srcUsed; i++) {
        target->hashTable->entries[targetUsed + i].value = src->hashTable->entries[i].value;
        target->hashTable->entries[targetUsed + i].key = *&src->hashTable->entries[i].key;
    }
    target->used = newUsed;
}

void map_print(BalMap *ptr) {
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
