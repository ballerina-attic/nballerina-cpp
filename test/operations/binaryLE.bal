// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    int a = 25;
    int b = 42;
    if (a <= b) {
        printu32(a);
    }
    else {
        printu32(b);
    }
}
// CHECK: RETVAL=25
