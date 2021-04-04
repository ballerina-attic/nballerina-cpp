// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

function foo(string? s) returns string {
    return <string>s;
}

public function main() {
    print_string("RESULT=");
    print_string(foo("Test"));
}
// CHECK: RESULT=Test
