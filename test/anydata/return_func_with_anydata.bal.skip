// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

string s = "";

function fooAnyData() returns anydata[] {
    s = "Test"; // TODO remove ressaignment when global init is fixed
    anydata[] ax = [];
    ax[0] = s;
    return ax;
}

public function main() {
    print_string("RESULT=");
    anydata[] ax2 = fooAnyData();
    string i = <string>ax2[0];
    print_string(i);
}
// CHECK: RESULT=Test
