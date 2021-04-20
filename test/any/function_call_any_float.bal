// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function bar(any z) returns float {
    float fl = <float>z;
    return fl;
}

public function main() {
    float b = 10.5;
    float c = bar(b);
    print_string("RESULT=");
    printf64(c);
}

// CHECK: RESULT=10.5
