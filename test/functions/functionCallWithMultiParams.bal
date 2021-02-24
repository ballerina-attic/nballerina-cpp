// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function bar(int x, int y, int z) returns int
{
    return x + y * z;
}

public function main() {
    int a = 5;
    int b = 10;
    printu32(bar(a, b, a));
}
// CHECK: RETVAL=55
