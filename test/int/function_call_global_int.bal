// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function bar(int z) returns int {
    return z + 10;
}

int a = 5;
public function main() {
    a = 5;
    print_string("RESULT=");
    printu32(bar(a));
}
// CHECK: RESULT=15
