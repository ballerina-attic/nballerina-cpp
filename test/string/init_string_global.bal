// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

string str = "Hello";
string str2 = " World!";
public function main() {
    str = "Hello";    // TODO remove when global init is fixed
    str2 = " World!";    // TODO remove when global init is fixed
    print_string(str);
    print_string(str2);
}
// CHECK: Hello World!
