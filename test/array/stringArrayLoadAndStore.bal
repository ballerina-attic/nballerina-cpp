// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function main() {
    string[5] arr4 = [];
    arr4[0] = "Hello";
    arr4[1] = " World";
    string b = arr4[0];
    string c = arr4[1];
    print_str(b);
    print_str(c);
}
// CHECK: Hello World
