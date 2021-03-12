// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function bar(int z) returns int
{
    return z + 10;
}

public function main() {
    int a = 5;
    _bal_result = bar(a);
}
// CHECK: RETVAL=15
