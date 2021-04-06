// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

public function main() {
    boolean e = false;
    boolean f = true;
    print_string("RESULT=");
    if (e) {
        print_i64(5);
    }
    else {
        if (f) {
            print_i64(10);
        }
        else {
            print_i64(4);
        }
    }
}
// CHECK: RESULT=10
