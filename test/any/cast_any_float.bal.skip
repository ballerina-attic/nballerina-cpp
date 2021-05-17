// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    float a = 77.7;
    any b = a;
    float c = <float>b;
    print_string("RESULT=");
    printf64(c);
}
// CHECK: RESULT=77.7
