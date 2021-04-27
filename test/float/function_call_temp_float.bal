// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function bar(float z) returns float {
    return z + 10.0;
}

public function main() {
    print_string("RESULT=");
    printf64(bar(5.5));
}
// CHECK: RESULT=15.5
