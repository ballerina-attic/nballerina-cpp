// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function bar(int x, int y, int z) returns int
{
    return x + y * z;
}

public function main() {
    int a = 5;
    int b = 10;
    print_str("RESULT=");
    printu32(bar(a, b, a));
}
// CHECK: RESULT=55
