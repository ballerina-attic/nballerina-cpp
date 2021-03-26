// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function printf32(float val) = external;

function fooInt() returns any {
    return 221;
}

function fooBool() returns any {
    return true;
}

function fooFloat() returns any {
    return 0.0;
}

public function main() {
    print_str("RESULT=");
    int i = <int>fooInt();
    boolean b = <boolean>fooBool();
    if (b) {
        i = i + 1;
    }
    float f = <float>fooFloat();
    printu32(i+<int>f);
}

// CHECK: RESULT=222
