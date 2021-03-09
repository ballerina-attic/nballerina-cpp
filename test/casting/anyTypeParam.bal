// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function bar(any l) returns int
{
    return <int>l;
}

public function main() {
    int a = 35;
    _bal_result = bar(a);
}
// CHECK: RETVAL=35
