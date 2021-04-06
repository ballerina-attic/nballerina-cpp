// RUN: "%testRunScript" %s %nballerinacc "%java_path"  "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

function foo(int i) returns int? {
    return i + 1;
}

public function main() {
    int? s = foo(221);
    print_string("RESULT=");
    print_i64(<int>s);
}
// CHECK: RESULT=222
