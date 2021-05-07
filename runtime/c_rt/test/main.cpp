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
    auto *myMap = map_new();
    BString bstring1 = {.value = "1"};
    map_store(myMap, &bstring1, 42);
    BString bstring2 = {.value = "2"};
    map_store(myMap, &bstring2, 43);
    BString bstring3 = {.value = "3"};
    map_store(myMap, &bstring3, 44);
    BString bstring4 = {.value = "4"};
    map_store(myMap, &bstring4, 45);
    BString bstring5 = {.value = "5"};
    map_store(myMap, &bstring5, 46);
    BString bstring6 = {.value = "6"};
    map_store(myMap, &bstring6, 47);

    BalValue outVal = 0;
    BString bstringFind = {.value = "4"};
    bool ret = map_load(myMap, &bstringFind, &outVal);

    ASSERT_EQ(ret, true);
    ASSERT_EQ(outVal, 45);
}
