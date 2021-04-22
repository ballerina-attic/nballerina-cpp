// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

float a = 7.7;
float b = -7.7;

public function main() {
    a = 7.7;    // TODO remove ressaignment when global init is fixed
    b = -7.7;
    print_string("RESULT=");
    printf64(a);
    print_string("RESULT=");
    printf64(b);
}

// CHECK: RESULT=7.7
// CHECK: RESULT=-7.7
