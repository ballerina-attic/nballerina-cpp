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
