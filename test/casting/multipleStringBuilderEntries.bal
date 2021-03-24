// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 35;
    string s = "Hello";
    boolean b = true;
    float f = 1.25;
    any a1 = a;
    int a11 = <int>a1;

    any s1 = s;
    string s11 = <string>s1;

    any b1 = b;
    boolean b11 = <boolean>b1;

    any f1 = f;
    float f11 = <float>f1;
    if (b11) {
        print_string("RESULT=");
    }
    if (a11 == 35) {
        print_string(s11);
    }
}
// CHECK: RESULT=Hello
