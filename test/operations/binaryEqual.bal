// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    int a = 10;
    int b = 10;
    if (a == b) {
        _bal_result = 45;
    }
}
// CHECK: RETVAL=45
