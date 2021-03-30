// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

string[5] arr4 = [];
public function main() {
    arr4 = [];
    arr4[0] = "Hello";
    arr4[1] = " World";
    print_string(arr4[0]);
    print_string(arr4[1]);
}
// CHECK: Hello World
