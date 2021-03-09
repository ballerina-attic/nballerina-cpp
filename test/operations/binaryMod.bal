// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 3;
    int b = 4;
    int c = 0;
    c = a % b;
    print_str("RESULT=");
    printu32(c);
}
// CHECK: RESULT=3
