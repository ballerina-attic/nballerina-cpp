// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    int a = -3;
    int b = 0;
    b = ~a;
    _bal_result = b;
}
// CHECK: RETVAL=2
