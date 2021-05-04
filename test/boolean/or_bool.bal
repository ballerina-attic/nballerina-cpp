// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function main() {
    boolean a = true;
    boolean b = true;
    boolean c = false;

    c = a || b;
    print_string("RESULT=");
    print_boolean(c);
    
    a = false;
    c = a || b;
    print_string("RESULT=");
    print_boolean(c);

    b = false;
    c = a || b;
    print_string("RESULT=");
    print_boolean(c);

}
// CHECK: RESULT=true
// CHECK: RESULT=true
// CHECK: RESULT=false
