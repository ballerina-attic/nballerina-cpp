// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int[5][3] arr4 = [[], []];
    arr4[0][0] = 10;
    arr4[1][2] = 20;
    any i1 = arr4[0][0];
    int i = <int>i1;
    int c = arr4[1][2];
    print_string("RESULT=");
    printu32(i + c);
}
// CHECK: RESULT=30
