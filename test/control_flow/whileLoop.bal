// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    int i = 0;
    while (i < 5) {
        i = i + 2;
    }
    _bal_result = i;
}
// CHECK: RETVAL=6
