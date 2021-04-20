// XFAIL
// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s
public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function print_integer(int val) = external;

public function main() {
    anydata x = 34;
    print_string("RESULT=");
    print_integer(<int>x);
    x = true;
    print_string("RESULT=");
    print_boolean(<boolean>x);
    x = "Anydata string";
    print_string("RESULT=");
    print_string(<string>x);
}
// CHECK: RESULT=34
// CHECK: RESULT=true
// CHECK: RESULT=Anydata string
// XFAIL: *