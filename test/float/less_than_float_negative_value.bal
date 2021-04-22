// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    float a = -42.85;
    float b = -22.56;
    print_string("RESULT=");
    if (a < b) {
        printf64(a);
    }
    else {
        printf64(b);
    }
}
// CHECK: RESULT=-42.85
