// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function main() {
    string str = "Hello";
    string str2 = " World!";
    print_string(str);
    print_string(str2);
}
// CHECK: Hello World!
