// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    float a = 10.1;
    float b = 5.5;
    float c = 2.0;

    if (a == 10.1) {
        c = b;
    }

    float d = 0.0;
    while (d < 3.3) {
        d = d + c;
    }

    float e = 4.4;
    float f = a * b * d;
    boolean g = false;
    float h = 0.0;

    g = h < 3.0;

    if (g) {
        h = f;
    }
    print_string("RESULT=");
    printf64(h);
}
// CHECK: RESULT=305.525
