// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function main() {
    boolean a = false;
    print_string("RESULT=");
    print_boolean(a);
    boolean b = true;
    print_string("RESULT=");
    print_boolean(b);
}
// CHECK: RESULT=false
// CHECK: RESULT=true
