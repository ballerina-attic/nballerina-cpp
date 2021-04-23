// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    float a = 10.3;
    float b = 5.7;
    print_string("RESULT=");
    printf64(a * b);
    float c = -10.3;
    float d = -5.7;
    print_string("RESULT=");
    printf64(c * d);
}
// CHECK: RESULT=58.71
// CHECK: RESULT=58.71000000000001
