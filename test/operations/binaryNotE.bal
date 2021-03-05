// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    if (a != b) {
        printu32(54);
    }
}
// CHECK: RETVAL=54

