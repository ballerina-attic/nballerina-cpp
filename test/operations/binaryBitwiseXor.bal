// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 0;
    int b = 1;
    int c = a ^ b;
    print_str("RESULT=");
    printu32(c);
}
// CHECK: RESULT=1
