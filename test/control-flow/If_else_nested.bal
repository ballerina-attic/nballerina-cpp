// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    boolean e = false;
    boolean f = true;
    print_string("RESULT=");
    if (e) {
        print_integer(5);
    }
    else {
        if (f) {
            print_integer(10);
        }
        else {
            print_integer(4);
        }
    }
}
// CHECK: RESULT=10
