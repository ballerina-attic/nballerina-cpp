// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    int a = -3;
    int b = 0;
    b = ~a;
    printu32(b);
}
// CHECK: RETVAL=2
