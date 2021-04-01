// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

anydata[4] ax1 = [];

public function main() {
    ax1 = []; // TODO remove ressaignment when global init is fixed
    ax1[0] = 23;
    ax1[1] = 239;
    int i1 = <int>ax1[0];
    int i2 = <int>ax1[1];
    print_string("RESULT=");
    printu32(i1);
    print_string("RESULT=");
    printu32(i2);
}
// CHECK: RESULT=23
// CHECK: RESULT=239
