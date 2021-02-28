// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;

function foo() returns any {
    return 221;
}

public function main() {
    int i = <int>foo();
    _bal_result = i;
}
// CHECK: RETVAL=221
