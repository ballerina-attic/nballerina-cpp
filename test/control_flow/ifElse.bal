// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    boolean e = false;
    boolean f = true;
    print_str("RESULT=");
    if (f) {
        printu32(20);
    }
    else {
        printu32(10);
    }
}
// CHECK: RESULT=20
