// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    boolean e = false;
    boolean f = false;
    e = !f;
    if (e) {
        printu32(0);
    }
    else {
        printu32(1);
    }
}
// CHECK: RETVAL=0
