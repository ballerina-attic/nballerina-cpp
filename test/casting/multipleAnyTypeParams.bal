// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function bar(any l, int m, any n) returns int
{
    return <int>l + m + <int>n;
}

public function main() {
    int a = 35;
    int b = 2;
    int c = 10;
    print_str("RESULT=");
    printu32(bar(a, b, c));
}
// CHECK: RESULT=47
