// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function main() {
    boolean e = false;
    boolean f = false;
    e = !f;
    print_string("RESULT=");
    print_boolean(e);
    boolean g = true;
    boolean h = true;
    h = !g;
    print_string("RESULT=");
    print_boolean(h);
}
// CHECK: RESULT=true
// CHECK: RESULT=false
