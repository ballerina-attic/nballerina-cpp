// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 35;
    string s = "Hello";
    boolean b = true;
    float f= 1.25;
    any a1 = a;
    int a11 = <int>a1;
    
    any s1 = s;
    string s11 = <string>s1;

    any b1 = b;
    boolean b11 = <boolean>b1;

    any f1 = f;
    float f11 = <float>f1;
    if (b11) {
    	print_str("RESULT=");
     	print_str(s11);
    }
}
// CHECK: RESULT=Hello
