// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function bar(int x, int y, int z) returns int
{
    return x + y * z;
}

public function main() {
    int a = 5;
    int b = 10;
    _bal_result = bar(a, b, a);
}
// CHECK: RETVAL=55
