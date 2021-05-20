// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function bar(any l) returns string {
    string g = <string>l;
    return g;
}
public function main() {
    string[5] arr4 = [];
    arr4[0] = "Hello";
    arr4[1] = " World";
    string b = bar(arr4[0]);
    string c = arr4[1];
    print_string(b);
    print_string(c);
}
// CHECK: Hello World
