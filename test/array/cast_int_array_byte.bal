// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_byte(byte val) = external;

public function main() {
    int[] i = [3];
    i[2] = 10;
    byte[] j = <byte[]>i;
    print_string("RESULT=");
    print_byte(j[2]);
}
// CHECK: RESULT=10
