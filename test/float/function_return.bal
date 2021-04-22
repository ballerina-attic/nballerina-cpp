// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function bar(float a, float b) returns float
{
    return b;
}

public function main() {
    float c = bar(7.5, 10.5);
    print_string("RESULT=");
    printf64(c);
}

// CHECK: RESULT=10.5
