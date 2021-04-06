// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int i = 0;
    while (i < 5) {
        i = i + 2;
    }
    print_string("RESULT=");
    print_integer(i);
}
// CHECK: RESULT=6
