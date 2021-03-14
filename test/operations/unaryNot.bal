// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    boolean e = false;
    boolean f = false;
    e = !f;
    if (e) {
        _bal_result = 0;
    }
    else {
        _bal_result = 1;
    }
}
// CHECK: RETVAL=0
