// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    boolean[5] arr4 = [];
    arr4[0] = true;
    any b1 = arr4[0];
    boolean b = <boolean>b1;
    if (b) {
        print_str("RESULT=");
        printu32(1);
    }
}
// CHECK: RESULT=1
