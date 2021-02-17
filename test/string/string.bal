// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    string str = "Hello World!";
}
// CHECK: RETVAL=0
