// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf32(float val) = external;

public function main() {
    float a = 10.5;
    float b = 5.5;
    print_string("RESULT=");
    if (a >= b) {
        printf32(a);
    }
    else {
        printf32(b);
    }
}
// CHECK: RESULT=10.5

