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
