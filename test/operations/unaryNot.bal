// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    boolean e = false;
    boolean f = false;
    e = !f;
    print_str("RESULT=");
    if (e) {
        printu32(0);
    }
    else {
        printu32(1);
    }
}
// CHECK: RESULT=0
