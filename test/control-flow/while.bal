// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

public function main() {
    int i = 0;
    while (i < 5) {
        i = i + 2;
    }
    print_string("RESULT=");
    printu64(i);
}
// CHECK: RESULT=6
