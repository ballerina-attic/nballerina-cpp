// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function main() {
    int|string u = "test string";
    string s = <string>u;
    print_str("RESULT=");
    print_str(s);
}
// CHECK: RESULT=test string
