#include <iostream>
using namespace std;

extern "C" {
extern void **new_string(const char *, int);
extern void print_string(void **);
}

int main() {
    std::string t = "RESULT=";
    void **temp = new_string(t.c_str(), t.length());
    print_string(temp);
}
