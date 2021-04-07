// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

function fooInt() returns int {
    anydata[] ax = [2];
    ax[0] = 45;
    return <int>ax[0];
}

function fooString() returns string {
    anydata[] ax = [2];
    ax[0] = "Test1";
    return <string>ax[0];
}

public function main() {
    print_string("RESULT=");
    printu32(fooInt());
    print_string(fooString());
}
// CHECK: RESULT=45
// CHECK: Test1
