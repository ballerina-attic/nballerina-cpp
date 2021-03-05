// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function bar(any l) returns int
{
    return <int>l;
}

public function main() {
    int a = 35;
    printu32(bar(a));
}
// CHECK: RETVAL=35
