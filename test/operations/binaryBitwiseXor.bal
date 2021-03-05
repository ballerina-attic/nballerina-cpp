// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    int a = 0;
    int b = 1;
    int c = a ^ b;
    printu32(c);
}
// CHECK: RETVAL=1
