// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function main() {
    boolean a = false;
    boolean b = true;
    any a1 = a;
    any b1 = b;
    boolean a11 = <boolean>a1;
    boolean b11 = <boolean>b1;

    print_string("RESULT=");
    print_boolean(a11);
    print_string("RESULT=");
    print_boolean(b11);

}
// CHECK: RESULT=false
// CHECK: RESULT=true
