// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    string str = "Hello World!";
    printu32(1);
}
// CHECK: RETVAL=1
