// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

function fooInt() returns any {
    return 221;
}

function fooBool() returns any {
    return true;
}

public function main() {
    print_str("RESULT=");
    int i = <int>fooInt();
    boolean b = <boolean>fooBool();
    if (b) {
        i = i + 1;
    }
    printu32(i);
}

// CHECK: RESULT=222
