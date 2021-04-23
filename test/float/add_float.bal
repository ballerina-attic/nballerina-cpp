// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    float a = 10.5;
    float b = 5.8;
    print_string("RESULT=");
    printf64(a + b);
    float c = -10.5;
    float d = -5.8;
    print_string("RESULT=");
    printf64(c + d);
}

// CHECK: RESULT=16.3
// CHECK: RESULT=-16.3
