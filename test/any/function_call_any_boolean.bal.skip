// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function bar(any z) returns boolean {
    return <boolean>z;
}

public function main() {
    boolean a = true;
    boolean b = bar(a);

    boolean c = false;
    boolean d = bar(c);

    print_string("RESULT=");
    print_boolean(b);
    print_string("RESULT=");
    print_boolean(c);
}

// CHECK: RESULT=true
// CHECK: RESULT=false
