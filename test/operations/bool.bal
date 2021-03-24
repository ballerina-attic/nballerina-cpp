// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    boolean f = false;
    boolean e = true;
    print_str("RESULT=");
    if (f) {
        printu32(0);
    }
    else {
        if (e) {
            printu32(42);
        }
        else {
            printu32(1);
        }
    }
}
// CHECK: RESULT=42
