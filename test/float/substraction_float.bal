// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf32(float val) = external;

public function main() {
    float a = 10.253;
    float b = 5.685;
    print_string("RESULT=");
    printf32(a - b);
}
// CHECK: RESULT=4.5680003
