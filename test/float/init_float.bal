// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    float a = 58.42;
    print_string("RESULT=");
    printf64(a);
    float b = -42.68;
    print_string("RESULT=");
    printf64(b);
}
// CHECK: RESULT=58.42
// CHECK: RESULT=-42.68
