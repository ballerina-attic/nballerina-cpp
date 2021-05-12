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

static void bal_map_init(BalMapPtr map, size_t min_capacity) {
    map->used = 0;
    size_t n = INITIAL_SIZE;
    while (n <= min_capacity) {
        n <<= 1;
        if (n == 0) {
            fprintf(stderr, "Capacity overflow!\n");
            abort();
        }
    }
    map->capacity = n;
    map->n_entries = n;
    map->entries = zalloc(n, sizeof(BalHashEntry));
}

BalMapPtr bal_map_create(void) {
    BalMapPtr map = (BalMapPtr)alloc_value(sizeof(BalMap));
    bal_map_init(map, INITIAL_SIZE);
    map->header.tag = HEADER_TAG_MAPPING;
    return map;
}

static void bal_map_grow(BalMapPtr map) {
    size_t newCapacity = map->capacity << 1;
    if (newCapacity == 0) {
        fprintf(stderr, "Capacity overflow!\n");
        abort();
    }
    map->entries = re_alloc(map->entries, sizeof(BalHashEntry), newCapacity);
    map->capacity = newCapacity;
    map->n_entries = newCapacity;
}

void bal_map_insert(BalMapPtr map, BalStringPtr key, BalValue value) {
    size_t currentUsed = map->used;
    BalHashEntry *entries = map->entries;
    entries[currentUsed].key = key;
    entries[currentUsed].value = value;
    map->used++;
    if (map->used >= map->capacity) {
        bal_map_grow(map);
    }
}

bool bal_map_lookup(BalMapPtr ptr, BalStringPtr key, BalValue *outValue) {
    bool retVal = false;
    size_t used = ptr->used;
    for (size_t i = 0; i < used; ++i) {
        if (compareBString(ptr->entries[i].key, key)) {
            retVal = true;
            *outValue = ptr->entries[i].value;
            break;
        }
    }
    return retVal;
}

void map_spread_field_init(BalMapPtr target, BalMapPtr src) {

    size_t srcUsed = src->used;
    size_t targetUsed = target->used;
    if (srcUsed > (SIZE_MAX - targetUsed)) {
        fprintf(stderr, "Capacity overflow!\n");
        abort(); // abort if size_t overflows
    }

    size_t newUsed = srcUsed + targetUsed;
    if (newUsed > target->capacity) {
        size_t newCapacity = target->capacity;
        while (newUsed > newCapacity) {
            newCapacity <<= 1;
            if (newCapacity == 0) {
                fprintf(stderr, "Capacity overflow!\n");
                abort();
            }
        }
        target->entries = re_alloc(target->entries, sizeof(BalHashEntry), newCapacity);
        target->capacity = newCapacity;
        target->n_entries = newCapacity;
    }

    for (size_t i = 0; i < srcUsed; i++) {
        target->entries[targetUsed + i].value = src->entries[i].value;
        target->entries[targetUsed + i].key = src->entries[i].key;
    }
    target->used = newUsed;
}
