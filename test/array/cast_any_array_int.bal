// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int[] i = [3];
    i[2] = 10;
    any ix = i;
    int[] j = <int[]>ix;
    print_string("RESULT=");
    printu32(j[2]);
}
// CHECK: RESULT=10
