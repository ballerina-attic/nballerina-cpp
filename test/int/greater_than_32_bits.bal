// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

public function main() {
    int a = 9223372036854775806;
    print_string("RESULT=");
    print_i64(a);
    int b = -9223372036854775807;
    print_string("RESULT=");
    print_i64(b);
}
// CHECK: RESULT=9223372036854775806
// CHECK: RESULT=-9223372036854775807
