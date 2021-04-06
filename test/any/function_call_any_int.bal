// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

public function bar(any l) returns int {
    return <int>l;
}

public function main() {
    int a = 35;
    print_string("RESULT=");
    print_i64(bar(a));
}
// CHECK: RESULT=35
