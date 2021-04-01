// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

string[5][5] arr4 = [[], []];
public function main() {
    arr4 = [[], []];
    arr4[0][2] = "Hello";
    arr4[1][3] = " World";
    print_string(arr4[0][2]);
    print_string(arr4[1][3]);
}
// CHECK: Hello World
