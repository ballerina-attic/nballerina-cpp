// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    boolean e = false;
    boolean f = true;
    print_string("RESULT=");
    if (e) {
        printu32(5);
    }
    else {
        if (f) {
            printu32(10);
        }
        else {
            printu32(4);
        }
    }
}
// CHECK: RESULT=10
