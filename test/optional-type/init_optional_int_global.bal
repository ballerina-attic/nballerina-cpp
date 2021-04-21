// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

int? a = 4;

public function main() {
    a = 24;
    print_string("RESULT=");
    print_integer(<int>a);
}
// CHECK: RESULT=24
