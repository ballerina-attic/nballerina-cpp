// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function bar(any l, int m, any n) returns int {
    return <int>l + m + <int>n;
}

public function main() {
    int a = 35;
    int b = 2;
    int c = 10;
    print_string("RESULT=");
    printu32(bar(a, b, c));
}
// CHECK: RESULT=47
