// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

function foo(string? s) returns string {
    return <string>s;
}

public function main() {
    print_string("RESULT=");
    print_string(foo("Test"));
}
// CHECK: RESULT=Test
