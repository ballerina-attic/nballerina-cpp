// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_byte(byte val) = external;

public function main() {
    byte a = 58;
    print_string("RESULT=");
    print_byte(a);
}
// CHECK: RESULT=58
