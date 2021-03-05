// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    int c = 8;
    boolean e = false;
    boolean f = true;
    if (a > b) {
        if (f) {
            printu32(a);
        }
        else {
            printu32(b);
        }
    }
    else {
        printu32(c);
    }
}
// CHECK: RETVAL=10
