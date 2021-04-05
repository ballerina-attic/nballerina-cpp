// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

public function main() {
    boolean e = false;
    boolean f = true;
    print_string("RESULT=");
    if (e) {
        printu64(5);
    }
    else {
        if (f) {
            printu64(10);
        }
        else {
            printu64(4);
        }
    }
}
// CHECK: RESULT=10
