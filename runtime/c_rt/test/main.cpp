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

extern "C" {
#include "include/balmap.h"
}
#include <gtest/gtest.h>

TEST(balmapTest1, crtTest) {
    auto *myMap = bal_map_create();
    BalString bstring1 = {.header = 1, .n_bytes = 1, .value = "1"};
    bal_map_insert(myMap, &bstring1, 42);
    BalString bstring2 = {.header = 1, .n_bytes = 1, .value = "2"};
    bal_map_insert(myMap, &bstring2, 43);
    BalString bstring3 = {.header = 1, .n_bytes = 1, .value = "3"};
    bal_map_insert(myMap, &bstring3, 44);
    BalString bstring4 = {.header = 1, .n_bytes = 1, .value = "4"};
    bal_map_insert(myMap, &bstring4, 45);
    BalString bstring5 = {.header = 1, .n_bytes = 1, .value = "5"};
    bal_map_insert(myMap, &bstring5, 46);
    BalString bstring6 = {.header = 1, .n_bytes = 1, .value = "6"};
    bal_map_insert(myMap, &bstring6, 47);

    BalValue outVal = 0;
    BalString bstringFind = {.header = 1, .n_bytes = 1, .value = "4"};
    bool ret = bal_map_lookup(myMap, &bstringFind, &outVal);

    ASSERT_EQ(ret, true);
    ASSERT_EQ(outVal, 45);
}

TEST(balmapTest2, crtTest) {

    auto *myMap = bal_map_create();
    const size_t iters = 100000;
    BalString bstrings[iters];

    for (auto i = 0; i < iters; i++) {
        auto *str = new char[8];
        sprintf(str, "%i", (int)i);
	bstrings[i].header = 1;
	bstrings[i].n_bytes = strlen(str);
        bstrings[i].value = str;
        bal_map_insert(myMap, &bstrings[i], (BalValue)i);
    }

    for (auto i = 0; i < iters; i++) {
        auto *str = new char[8];
        sprintf(str, "%i", (int)i);
        bstrings[i].header = 1;
        bstrings[i].n_bytes = strlen(str);
        bstrings[i].value = str;
        BalValue outVal = 0;
        bool ret = bal_map_lookup(myMap, &bstrings[i], &outVal);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(outVal, i);
    }
}
