// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;

public function main() {
    int | string var1 = 42;
    any result1 = <any>var1;
    _bal_result = <int>result1;
}
// CHECK: RETVAL=42
