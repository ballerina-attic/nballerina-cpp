// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    int a = 10;
    int b = 10;
    if (a == b) {
        printu32(45);
    }
}
// CHECK: RETVAL=45
