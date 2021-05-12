// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int[] i = [1,2];
    i[0] = 1;
    i[1] = 2;
    any[] a = i;
    any x = a[1];
    print_string("RESULT=");
    print_integer(<int>x);
}
// CHECK: RESULT=2
