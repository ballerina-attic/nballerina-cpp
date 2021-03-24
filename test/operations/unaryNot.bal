// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    boolean e = false;
    boolean f = false;
    e = !f;
    print_string("RESULT=");
    if (e) {
        printu32(0);
    }
    else {
        printu32(1);
    }
}
// CHECK: RESULT=0
