// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function bar(int z) returns int {
    return z + 10;
}

int a = 5;
public function main() {
    a = 5;
    print_string("RESULT=");
    print_integer(bar(a));
}
// CHECK: RESULT=15
