#include "include/balmap.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    struct BalMap *myMap = map_new();
    map_store(myMap, &(struct BString){.value = "1"}, 42);
    map_store(myMap, &(struct BString){.value = "2"}, 43);
    map_store(myMap, &(struct BString){.value = "3"}, 44);
    map_store(myMap, &(struct BString){.value = "4"}, 45);
    map_store(myMap, &(struct BString){.value = "5"}, 46);
    map_store(myMap, &(struct BString){.value = "6"}, 47);

    map_print(myMap);

    BalValue outVal = 0;
    if (map_load(myMap, &(struct BString){.value = "4"}, &outVal)) {
        printf("Found %zu\n", (uint64_t)outVal);
    } else {
        printf("Error: map look up failed!\n");
        abort();
    }

    return 0;
}
