// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

function foo() returns int|string {
    return 221;
}

public function main() {
    int i = <int>foo();
    print_str("RESULT=");
    printu32(i); 
}
// CHECK: RESULT=221
