// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

function foo(string? s) returns string {
    return <string>s;
}

public function main() {
    print_str("RESULT=");
    print_str(foo("Test"));
}
// CHECK: RESULT=Test
