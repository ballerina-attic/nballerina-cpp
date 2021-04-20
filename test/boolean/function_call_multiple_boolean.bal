// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function bar(boolean x, boolean y) returns boolean {
    return x && y ;
}

public function main() {
    boolean a = false;
    boolean b = true;
    print_string("RESULT=");
    print_boolean(bar(a, b));
}
// CHECK: RESULT=false
