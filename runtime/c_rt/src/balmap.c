/*
 * Copyright (c) 2021, WSO2 Inc. (http://www.wso2.org) All Rights Reserved.
 *
 * WSO2 Inc. licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "include/balmap.h"
#include "include/gc.h"
#include <stdio.h>

static const size_t INITIAL_SIZE = 8;
static const float LOAD_FACTOR = 0.6f;
static const size_t HASH_TABLE_MIN_SIZE = 8;

static void bal_map_init_hashtable(BalMapPtr map, size_t min_capacity) {
    map->used = 0;
    size_t n = HASH_TABLE_MIN_SIZE;
    while ((size_t)(n * LOAD_FACTOR) < min_capacity) {
        n <<= 1;
        if (n == 0) {
            fprintf(stderr, "Capacity overflow!\n");
            abort();
        }
    }
    map->capacity = (size_t)(n * LOAD_FACTOR);
    map->n_entries = n;
    map->entries = zalloc(map->n_entries, sizeof(BalHashEntry));
}

BalMapPtr bal_map_create(void) {
    BalMapPtr map = (BalMapPtr)alloc_value(sizeof(BalMap));
    bal_map_init_hashtable(map, INITIAL_SIZE);
    map->header.tag = HEADER_TAG_MAPPING;
    return map;
}

static void bal_map_grow_hashtable(BalMapPtr map, size_t new_size) {
    BalMap nMap;
    if (new_size <= map->used) {
        fprintf(stderr, "Map shrinking is not supported!\n");
        abort();
    }
    bal_map_init_hashtable(&nMap, new_size);

    BalHashEntry *entries = map->entries;
    size_t n = map->n_entries;
    for (size_t i = 0; i < n; i++) {
        if (entries[i].key != 0) {
            bal_map_insert(&nMap, entries[i].key, entries[i].value);
        }
    }
    map->used = nMap.used;
    map->capacity = nMap.capacity;
    map->n_entries = nMap.n_entries;
    map->entries = nMap.entries;
}

static void bal_map_insert_with_hash(BalMapPtr map, BalStringPtr key, BalValue value, unsigned long hash) {
    size_t i = hash & (map->n_entries - 1);
    if (i >= map->n_entries) {
        fprintf(stderr, "Unexpected hashing failure!\n");
        abort();
    }

    BalHashEntry *entries = map->entries;
    for (;;) {
        if (entries[i].key == 0) {
            break;
        }
        if (bal_string_equals(key, entries[i].key)) {
            entries[i].value = value;
            return;
        }
        if (i > 0) {
            --i;
        } else {
            i = map->n_entries - 1;
        }
    }
    entries[i].value = value;
    entries[i].key = key;
    map->used += 1;
    if (map->used >= map->capacity) {
        bal_map_grow_hashtable(map, map->used + 1);
    }
}

void bal_map_insert(BalMapPtr map, BalStringPtr key, BalValue value) {
    bal_map_insert_with_hash(map, key, value, bal_string_hash(key));
}

static bool bal_map_lookup_with_hash(BalMapPtr map, BalStringPtr key, BalValue *outValue, unsigned long hash) {
    size_t i = hash & (map->n_entries - 1);
    if (i >= map->n_entries) {
        fprintf(stderr, "Unexpected hashing failure!\n");
        abort();
    }

    BalHashEntry *entries = map->entries;
    for (;;) {
        if (entries[i].key == 0) {
            return false;
        }
        if (bal_string_equals(key, entries[i].key)) {
            *outValue = entries[i].value;
            return true;
        }
        if (i > 0) {
            --i;
        } else {
            i = map->n_entries - 1;
        }
    }
}

bool bal_map_lookup(BalMapPtr map, BalStringPtr key, BalValue *outValue) {
    return bal_map_lookup_with_hash(map, key, outValue, bal_string_hash(key));
}

void map_spread_field_init(BalMapPtr target, BalMapPtr src) {

    size_t srcUsed = src->used;
    size_t targetUsed = target->used;
    if (srcUsed > (SIZE_MAX - targetUsed)) {
        fprintf(stderr, "Capacity overflow!\n");
        abort(); // abort if size_t overflows
    }
    size_t newUsed = srcUsed + targetUsed;
    bal_map_grow_hashtable(target, newUsed);

    BalHashEntry *entries = src->entries;
    for (size_t i = 0; i < srcUsed; i++) {
        if (entries[i].key != 0) {
            bal_map_insert(target, entries[i].key, entries[i].value);
        }
    }
}
