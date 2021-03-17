// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    int c = 8;
    boolean e = false;
    boolean f = true;
    print_str("RESULT=");
    if (a > b) {
        if (f) {
            printu32(a);
        }
        else {
            printu32(b);
        }
    }
    else {
        printu32(c);
    }
}
// CHECK: RESULT=10
