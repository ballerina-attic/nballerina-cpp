// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

public function main() {
    boolean f = true;
    print_string("RESULT=");
    if (f) {
        print_i64(20);
    }
    else {
        print_i64(10);
    }
}
// CHECK: RESULT=20
