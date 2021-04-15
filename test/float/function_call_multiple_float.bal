// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf32(float val) = external;

public function bar(float x, float y, float z) returns float {
    return x + y * z;
}

public function main() {
    float a = 5.5;
    float b = 10.7;
    print_string("RESULT=");
    printf32(bar(a, b, a));
}
// CHECK: RESULT=64.35
