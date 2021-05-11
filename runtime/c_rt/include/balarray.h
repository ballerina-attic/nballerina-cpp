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
#ifndef __BALARRAY__H__
#define __BALARRAY__H__

#include <stdint.h>
#include <stdlib.h>

typedef struct DynamicArray {
    uint64_t header;
    void *values;
} DynamicArray;

typedef struct DynamicBalArray {
    uint64_t header;
    uint32_t inherent_type;
    uint64_t length;
    uint64_t capacity;
    DynamicArray *array;
} DynamicBalArray;

int64_t array_load_int(DynamicBalArray *arry_ptr, int64_t index);
#endif //!__BALARRAY__H__
