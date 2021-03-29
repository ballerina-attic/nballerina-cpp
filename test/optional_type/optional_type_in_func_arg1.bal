// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

function foo(int? i) returns int {
    return <int>i;
}

public function main() {
    print_string("RESULT=");
    printu32(foo(32));
}
// CHECK: RESULT=32
