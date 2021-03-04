// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;

public function main() {
    int|string u = 21;
    int i = <int>u;
    _bal_result = i;
}
// CHECK: RETVAL=21
