// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

public function main() {
    int a = 10;
    int b = 10;
    if (a == b) {
        print_string("RESULT=");
        printu64(45);
    }
}
// CHECK: RESULT=45
