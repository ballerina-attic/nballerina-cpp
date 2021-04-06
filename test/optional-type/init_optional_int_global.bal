// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

int? a = 4;

public function main() {
    a = 24;
    print_string("RESULT=");
    print_i64(<int>a);
}
// CHECK: RESULT=24
