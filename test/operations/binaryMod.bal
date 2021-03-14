// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    int a = 3;
    int b = 4;
    int c = 0;
    c = a % b;
    _bal_result = c;
}
// CHECK: RETVAL=3
