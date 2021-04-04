// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    print_string("RESULT=");
    if (a >= b) {
        printu32(a);
    }
    else {
        printu32(b);
    }
}
// CHECK: RESULT=10

