// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function bar(any l) returns int
{
    return <int>l;
}

public function main() {
    int a = 35;
    print_string("RESULT=");
    printu32(bar(a));
}
// CHECK: RESULT=35
