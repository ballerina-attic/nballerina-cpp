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

#ifndef __GC__H__
#define __GC__H__

#include "include/common.h"
#include <stdint.h>
#include <stdio.h>

#ifdef _MSC_VER
// MSC does not support aligned_alloc
// _aligned_malloc is like aligned_alloc except that
// the allocated memory has to be freed with _aligned_free
// Since we're not freeing anything, this doesn't affect us
#define aligned_alloc(a, n) _aligned_malloc(n, a)
#endif

static BalHeader *alloc_value(size_t n_bytes) {
    void *mem = aligned_alloc(8, n_bytes);
    if (mem == NULL) {
        fprintf(stderr, "Memory allocation failed \n");
        abort();
    }
    BalHeaderPtr h = mem;
    h->tag = HEADER_TAG_UNINIT;
    return h;
}

static void *zalloc(size_t n_members, size_t member_size) {
    void *mem = calloc(n_members, member_size);
    if (mem == NULL) {
        fprintf(stderr, "Memory allocation failed \n");
        abort();
    }
    return mem;
}

#endif //!__GC__H__
