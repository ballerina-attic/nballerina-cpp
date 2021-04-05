// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

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
    print_string("RESULT=");
    printu64(h);
}
// CHECK: RESULT=250
