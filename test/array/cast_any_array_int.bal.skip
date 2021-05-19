// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int[] i = [3];
    i[2] = 10;
    any ix = i;
    int[] j = <int[]>ix;
    print_string("RESULT=");
    print_integer(j[2]);
}
// CHECK: RESULT=10
