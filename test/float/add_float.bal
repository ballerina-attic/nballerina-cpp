// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf32(float val) = external;

public function main() {
    float a = 10.5;
    float b = 5.8;
    print_string("RESULT=");
    printf32(a + b);
}

// CHECK: RESULT=16.3

