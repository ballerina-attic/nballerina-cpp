// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int a = 10;
    int b = 10;
    if (a == b) {
        print_string("RESULT=");
        print_integer(45);
    }
}
// CHECK: RESULT=45
