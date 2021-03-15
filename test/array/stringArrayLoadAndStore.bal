// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s


public function print_str(string val) = external;

public function main() {
    string[5] arr4 = [];
    arr4[0] = "Hello";
    any b1 = arr4[0];
    string b = <string>b1;
    print_str("RESULT=");
    print_str(b);
}
// CHECK: RESULT=Hello
