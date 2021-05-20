// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function print_integer(int val) = external;

function fooInt1(anydata x, anydata y) returns int {
    return <int>x + <int>y;
}

function fooInt2(anydata x, anydata y) returns anydata {
    return <int>x + <int>y;
}

function fooBool(anydata x, boolean b) returns boolean {
    if (<boolean>x) {
        if (b) {
            return true;
        }
    }
    return false;
}

function fooString(anydata s) returns string {
    return <string>s;
}

public function main() {
    print_string("RESULT=");
    print_integer(fooInt1(2, 3));

    print_string("RESULT=");
    print_integer(<int>fooInt2(20, 3));

    print_string("RESULT=");
    print_boolean(fooBool(true, true));
    print_string("RESULT=");
    print_boolean(fooBool(true, false));

    print_string("RESULT=");
    print_string(fooString("Anydata string"));
}
// CHECK: RESULT=5
// CHECK: RESULT=23
// CHECK: RESULT=true
// CHECK: RESULT=false
// CHECK: RESULT=Anydata string
