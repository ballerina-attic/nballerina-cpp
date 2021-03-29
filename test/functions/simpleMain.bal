// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    print_string("RESULT=");
    printu32(128);
}
// CHECK: RESULT=128
