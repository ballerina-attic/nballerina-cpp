// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function bar(any l, int m, any n) returns int
{
    return <int>l + m + <int>n;
}

public function main() {
    int a = 35;
    int b = 2;
    int c = 10;
    printu32(bar(a, b, c));
}
// CHECK: RETVAL=47
