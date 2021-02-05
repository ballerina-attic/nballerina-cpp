// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    int a = 10;
    int b = 5;
    int c = 8;
    boolean e = false;
    boolean f = true;
    if (a > b) {
        if (f) {
            _bal_result = a;
        }
        else {
            _bal_result = b;
        }
    }
    else {
        _bal_result = c;
    }
}
// CHECK: RETVAL=10
