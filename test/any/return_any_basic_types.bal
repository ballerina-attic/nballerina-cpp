// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print64(int val) = external;

public function printf32(float val) = external;

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
    print64(i);
    float f = <float>fooFloat();
    print_string("RESULT=");
    printf32(f);
}

// CHECK: RESULT=222
// CHECK: RESULT=7.1
