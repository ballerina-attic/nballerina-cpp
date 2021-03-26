// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 40;
    any b = a;
    int c = <int>b;
    print_str("RESULT=");
    printu32(c);
}
// CHECK: RESULT=40
