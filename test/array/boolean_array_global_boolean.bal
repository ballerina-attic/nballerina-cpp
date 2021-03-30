// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

boolean[5] arr4 = [];
public function main() {
    arr4 = [];
    arr4[0] = true;
    arr4[1] = true;
    boolean b = arr4[0];
    boolean c = arr4[1];
    if (b) {
        print_string("RESULT=");
    }
    if (c) {
        printu32(1);
    }
}
// CHECK: RESULT=1
