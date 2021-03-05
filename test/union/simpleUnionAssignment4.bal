// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;

public function main() {
    any var1 = 42;
    int result1 = <int>var1;
    int | string var2 = 7;
    int result2 = <int>var2;
    _bal_result = result1 + result2;
}

// CHECK: RETVAL=49
