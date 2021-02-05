// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    int a = 58;
    _bal_result = a;
}
// CHECK: RETVAL=58
