// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    int a = 58;
    printu32(a);
}
// CHECK: RETVAL=58
