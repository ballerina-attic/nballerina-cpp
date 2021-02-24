// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function bar(int z) returns int
{
    return z + 10;
}

public function main() {
    int a = 5;
    printu32(bar(a));
}
// CHECK: RETVAL=15
