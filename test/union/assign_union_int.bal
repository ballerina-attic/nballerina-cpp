// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int|string u = 21;
    int i = <int>u;
    print_string("RESULT=");
    printu32(i);
}
// CHECK: RESULT=21
