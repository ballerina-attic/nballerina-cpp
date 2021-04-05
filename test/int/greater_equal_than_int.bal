// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    print_string("RESULT=");
    if (a >= b) {
        printu64(a);
    }
    else {
        printu64(b);
    }
}
// CHECK: RESULT=10

