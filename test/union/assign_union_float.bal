// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    float|string u = 21.2;
    float i = <float>u;
    print_string("RESULT=");
    printf64(i);
}
// CHECK: RESULT=21.2
