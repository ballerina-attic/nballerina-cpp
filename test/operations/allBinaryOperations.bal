// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    int a = 10;
    int b = 5;
    int c = 0;

    if (a == 10) {
        c = b;
    }

    int d = 0;
    while (d < 3) {
        d = d + c;
    }

    int e = 4;
    int f = a * b * d;
    boolean g = true;
    int h = 0;

    g = f > 5;
    if (g) {
        h = f;
    }
    _bal_result = h;
}
// CHECK: RETVAL=250
