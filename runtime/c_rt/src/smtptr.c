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

#include "include/smtptr.h"
#include "include/gc.h"
#include <stdio.h>

SmtPtr *getSmtPtrFromInt(int64_t value) {
    SmtPtr *ptr = ALLOC_FIXED_VALUE(SmtPtr);
    char* type = (char*)alloc_array(4,sizeof(char));
    strcpy(type, "__I\0");
    ptr->inherent_type_ptr = type;
    int64_t* val_ptr = ALLOC_FIXED_VALUE(int64_t);
    *val_ptr = value;
    ptr->value_ptr = val_ptr;
    return ptr;
}


SmtPtr *getSmtPtrFromByte(int8_t value) {
    SmtPtr *ptr = ALLOC_FIXED_VALUE(SmtPtr);
    char* type = (char*)alloc_array(4,sizeof(char));
    strcpy(type, "__Y\0");
    ptr->inherent_type_ptr = type;
    int64_t* val_ptr = ALLOC_FIXED_VALUE(int64_t);
    *val_ptr = value;
    ptr->value_ptr = val_ptr;
    return ptr;
}

void smt_ptrPrint(SmtPtr *ptr) {
    printf("pointer: %p\n", ptr);
    printf("value ptr: %p\n", ptr->value_ptr);
    printf("inherent type ptr: %p\n", ptr->inherent_type_ptr);
}
