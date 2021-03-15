// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    print_str("RESULT=");
    if (a >= b) {
        printu32(a);
    }
    else {
        printu32(b);
    }
}
// CHECK: RESULT=10

