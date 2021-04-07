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
    ax[0] = "Test1\n";
    return <string>ax[0];
}

function fooAnyData1() returns anydata[] {
    anydata[] ax = [];
    ax[0] = "Test2\n";
    return ax;
}

string s = "";
function fooAnyData2() returns anydata[] {
    s = "Test3\n"; // TODO remove ressaignment when global init is fixed
    anydata[] ax = [];
    ax[0] = s;
    return ax;
}

public function main() {
    print_string("RESULT=");
    printu32(fooInt());
    print_string(fooString());

    anydata[] ax1 = fooAnyData1();
    string i = <string>ax1[0];
    print_string(i);

    anydata[] ax2 = fooAnyData2();
    i = <string>ax2[0];
    print_string(i);
}
// CHECK: RESULT=45
// CHECK: Test1
// CHECK: Test2
// CHECK: Test3
