// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int[] i = [3];
    i[0] = 10;
    any ix = i;
    int[] j = <int[]>ix;
    print_str("RESULT=");
    printu32(j[0]);
}
// CHECK: RESULT=10
