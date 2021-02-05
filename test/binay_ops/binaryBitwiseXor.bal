// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    int a = 0;
    int b = 1;
    int c = a ^ b;
    _bal_result = c;
}
// CHECK: RETVAL=1
