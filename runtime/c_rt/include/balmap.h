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

#include "include/common.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    BalStringPtr key;
    BalValue value;
} BalHashEntry;

typedef struct {
    BalHeader header;
    // how many of entries are used
    size_t used;
    // used must be < capacity
    // capacity = LOAD_FACTOR * n_entries
    size_t capacity;
    // always a power of 2
    // length of entries array
    size_t n_entries;
    BalHashEntry *entries;
} BalMap, *BalMapPtr;

BalMapPtr bal_map_create(void);
void bal_map_insert(BalMapPtr map, BalStringPtr key, BalValue value);
bool bal_map_lookup(BalMapPtr map, BalStringPtr key, BalValue *outValue);
void map_spread_field_init(BalMapPtr target, BalMapPtr src);

#endif //!__BALMAP__H__
