// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int a = 35;
    string s = "Hello";
    boolean b = true;
    float f = 1.25;

    any a1 = a;
    any s1 = s;
    any b1 = b;
    any f1 = f;

    int a11 = <int>a1;
    string s11 = <string>s1;
    boolean b11 = <boolean>b1;
    float f11 = <float>f1;

    if (b11) {
        print_string("RESULT=");
    }
    if (a11 == 35) {
        print_string(s11);
    }
// TODO check float value
}
// CHECK: RESULT=Hello
