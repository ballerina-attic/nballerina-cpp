// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

function fooAnyData1() returns anydata[] {
    anydata[] ax = [];
    ax[0] = "Test1";
    return ax;
}

string s = "";

function fooAnyData2() returns anydata[] {
    s = " Test2"; // TODO remove ressaignment when global init is fixed
    anydata[] ax = [];
    ax[0] = s;
    return ax;
}

public function main() {
    anydata[] ax1 = fooAnyData1();
    string i = <string>ax1[0];
    print_string("RESULT=");
    print_string(i);

    anydata[] ax2 = fooAnyData2();
    i = <string>ax2[0];
    print_string(i);
}
// CHECK: RESULT=Test1 Test2
