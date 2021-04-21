// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function bar(string z) returns string {
    return z;
}

string str2 = " World!";

public function main() {
    string str = "Hello";
    print_string("RESULT=");
    print_string(str);
    str2 = " World!";    // TODO remove when global init is fixed
    print_string(bar(str2));
}
// CHECK: RESULT=Hello World!
