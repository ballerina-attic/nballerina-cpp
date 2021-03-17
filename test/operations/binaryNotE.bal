// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    if (a != b) {
        print_str("RESULT=");
        printu32(54);
    }
}
// CHECK: RESULT=54
