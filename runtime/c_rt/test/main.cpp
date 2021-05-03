extern "C" {
#include "include/balmap.h"
}
#include <gtest/gtest.h>

TEST(balmapTest1, crtTest) {
    auto *myMap = map_new();
    BString bstring1 = {.value = (char *)"1"};
    map_store(myMap, &bstring1, 42);
    BString bstring2 = {.value = (char *)"2"};
    map_store(myMap, &bstring2, 43);
    BString bstring3 = {.value = (char *)"3"};
    map_store(myMap, &bstring3, 44);
    BString bstring4 = {.value = (char *)"4"};
    map_store(myMap, &bstring4, 45);
    BString bstring5 = {.value = (char *)"5"};
    map_store(myMap, &bstring5, 46);
    BString bstring6 = {.value = (char *)"6"};
    map_store(myMap, &bstring6, 47);

    BalValue outVal = 0;
    BString bstringFind = {.value = (char *)"4"};
    bool ret = map_load(myMap, &bstringFind, &outVal);

    ASSERT_EQ(ret, true);
    ASSERT_EQ(outVal, 45);
}
