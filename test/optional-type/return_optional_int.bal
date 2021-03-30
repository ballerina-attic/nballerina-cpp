// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

function foo(int i) returns int? {
    return i + 1;
}

public function main() {
    int? s = foo(221);
    print_string("RESULT=");
    printu32(<int>s);
}
// CHECK: RESULT=222
