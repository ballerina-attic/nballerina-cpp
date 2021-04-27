// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

function fooFloat() returns any {
    return 1.0;
}

public function main() {
    float f = <float>fooFloat();
    print_string("RESULT=");
    print_integer(7 + <int>f);
}

// CHECK: RESULT=8
