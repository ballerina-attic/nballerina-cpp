// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function main() {
    string str = "Hello World!";
}
// CHECK: RETVAL=40
