// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

public function main() {
    int val = 8589934591;
    print_string("RESULT=");
    printu64(val);
}
// CHECK: RESULT=8589934591
