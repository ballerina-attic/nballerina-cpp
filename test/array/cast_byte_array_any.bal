// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_byte(byte val) = external;

public function main() {
    byte[] i = [1,2];
    i[0] = 1;
    i[1] = 2;
    any[] a = i;
    any x = a[1];
    print_string("RESULT=");
    print_byte(<byte>x);
}
// CHECK: RESULT=2
