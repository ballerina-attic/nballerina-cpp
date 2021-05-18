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

#ifndef __COMMON__H__
#define __COMMON__H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define HEADER_TAG_UNINIT 0
#define HEADER_TAG_INT 1
#define HEADER_TAG_STRING 2
#define HEADER_TAG_LIST 3
#define HEADER_TAG_MAPPING 4

// Types

typedef struct {
    uint8_t tag;
    uint8_t gc_reserved;
    uint16_t spare1;
    uint32_t spare2;
} BalHeader, *BalHeaderPtr;

typedef uint64_t BalValue;

typedef struct {
    uint64_t header;
    uint64_t n_bytes;
    const char *value;
} BalString, *BalStringPtr;

// Static functions
inline static bool bal_string_equals(BalStringPtr str1, BalStringPtr str2) {
    return strcmp(str1->value, str2->value) == 0;
}

// DJB2 hash function
static unsigned long bal_string_hash(BalStringPtr s) {
    unsigned long hash = 5381;
    const char *p = s->value;
    size_t n = strlen(p);
    while (n-- > 0) {
        hash = hash * 33 + (unsigned char)*p++;
    }
    return hash;
}

#endif //!__COMMON__H__
