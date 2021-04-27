// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function printf64(float val) = external;

function fooInt() returns any {
    return 221;
}

function fooBool() returns any {
    return true;
}

function fooFloat() returns any {
    return 7.1;
}

public function main() {
    print_string("RESULT=");
    int i = <int>fooInt();
    boolean b = <boolean>fooBool();
    if (b) {
        i = i + 1;
    }
    print_integer(i);
    float f = <float>fooFloat();
    print_string("RESULT=");
    printf64(f);
    print_string("RESULT=");
    print_integer(i + <int>f);
}

// CHECK: RESULT=222
// CHECK: RESULT=7.1
// CHECK: RESULT=229
