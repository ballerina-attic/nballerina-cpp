// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function bar(any l) returns string {
    string g = <string>l;
    return g;
}
public function main() {
    string[5] arr4 = [];
    arr4[0] = "Hello";
    string b = bar(arr4[0]);
    print_string("RESULT=");
    print_string(b);
}
// CHECK: RESULT=Hello
