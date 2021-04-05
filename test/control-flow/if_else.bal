// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

public function main() {
    boolean f = true;
    print_string("RESULT=");
    if (f) {
        printu64(20);
    }
    else {
        printu64(10);
    }
}
// CHECK: RESULT=20
