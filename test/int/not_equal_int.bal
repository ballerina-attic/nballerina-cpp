// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    if (a != b) {
        print_string("RESULT=");
        print_integer(54);
    }
}
// CHECK: RESULT=54
