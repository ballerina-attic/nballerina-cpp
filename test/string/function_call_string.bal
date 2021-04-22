// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function bar(string z) returns string {
    return z;
}

public function main() {
    string str = "Hello";
    print_string("RESULT=");
    print_string(str);
    string str2 = " World!";
    print_string(bar(str2));
}
// CHECK: RESULT=Hello World!
