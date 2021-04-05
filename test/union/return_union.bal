// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

function foo() returns int|string {
    return 221;
}

public function main() {
    int i = <int>foo();
    print_string("RESULT=");
    printu64(i);
}
// CHECK: RESULT=221
