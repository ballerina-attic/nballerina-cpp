// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;

public function main() {
    int|string u = "test string";
    string s = <string>u;
}
// CHECK: RETVAL=0
