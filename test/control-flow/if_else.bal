// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    boolean f = true;
    print_string("RESULT=");
    if (f) {
        print_integer(20);
    }
    else {
        print_integer(10);
    }
}
// CHECK: RESULT=20
