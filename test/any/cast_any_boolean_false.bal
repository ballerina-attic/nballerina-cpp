// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function main() {
    boolean a = false;
    any a1 = a;
    boolean a11 = <boolean>a1;
    print_string("RESULT=");
    print_boolean(a11);
}
// CHECK: RESULT=false
