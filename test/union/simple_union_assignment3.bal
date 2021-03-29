// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function main() {
    int|string u = "test string";
    string s = <string>u;
    print_string("RESULT=");
    print_string(s);
}
// CHECK: RESULT=test string
