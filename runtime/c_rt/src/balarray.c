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

#include "include/balarray.h"
#include <stdio.h>

void array_print(DynamicBalArray *ptr) {
    printf("header : %ld \n", ptr->header);
    printf("type : %d \n", ptr->inherent_type);
    printf("length : %ld \n", ptr->length);
    printf("capacity : %ld \n", ptr->capacity);
    printf("\t array header : %ld \n", ptr->array->header);
}

void *getItemAt(DynamicBalArray *array_ptr, int64_t index) {
    if (array_ptr->length <= index) {
        fprintf(stderr, "%s", "Index is out of range\n");
        abort();
    }
    uint64_t header = array_ptr->header;
    uint64_t header_type = header & 3;
    uint64_t* base_ptr = &(array_ptr->array->header) + 1;
    if (header_type == 0) {
        uint8_t* ptr = (uint8_t*) base_ptr;
        return ptr + index;
    } else if (header_type == 1) {
        uint16_t* ptr = (uint16_t*) base_ptr;
        return ptr + index;
    } else if (header_type == 2) {
        uint32_t* ptr = (uint32_t*) base_ptr;
        return ptr + index;
    } else {
        uint64_t* ptr = (uint64_t*) base_ptr;
        return ptr + index;
    }
}

int64_t castPointerToInteger(DynamicBalArray *array_ptr, void *ptr) {
    uint64_t header = array_ptr->header;
    uint64_t header_type = header & 3;
    if (header_type == 0) {
        uint8_t value = *(uint8_t *)ptr;
        return value;
    } else if (header_type == 1) {
        uint16_t value = *(uint16_t *)ptr;
        return value;
    } else if (header_type == 2) {
        uint32_t value = *(uint32_t *)ptr;
        return value;
    } else {
        uint64_t value = *(uint64_t *)ptr;
        return value;
    }
}

int64_t array_load_int(DynamicBalArray *array_ptr, int64_t index) {
    return castPointerToInteger(array_ptr, getItemAt(array_ptr, index));
}