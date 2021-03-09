// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = -3;
    int b = 0;
    b = ~a;
    print_str("RESULT=");
    printu32(b);
}
// CHECK: RESULT=2
