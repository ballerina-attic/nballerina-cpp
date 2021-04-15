// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf32(float val) = external;

public function main() {
    float a = 42.85;
    float b = 22.56;
    print_string("RESULT=");
    if (a < b) {
        printf32(a);
    }
    else {
        printf32(b);
    }
}
// CHECK: RESULT=22.56
