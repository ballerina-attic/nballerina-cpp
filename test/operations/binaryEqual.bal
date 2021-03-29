// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 10;
    int b = 10;
    if (a == b) {
        print_string("RESULT=");
        printu32(45);
    }
}
// CHECK: RESULT=45
