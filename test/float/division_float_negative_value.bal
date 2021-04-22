// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    float a = -10.10;
    float b = -5.55;
    print_string("RESULT=");
    printf64(a / b);
}
// CHECK: RESULT=1.8198198198198199
