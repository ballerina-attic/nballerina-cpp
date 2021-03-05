// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    boolean e = false;
    boolean f = true;
    if (f) {
        printu32(20);
    }
    else {
        printu32(10);
    }
}
// CHECK: RETVAL=20
