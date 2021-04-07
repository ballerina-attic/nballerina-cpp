// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    anydata[] ax1 = [];
    ax1[0] = 23;
    ax1[1] = 239;
    int i1 = <int>ax1[0];
    int i2 = <int>ax1[1];
    print_string("RESULT=");
    printu32(i1);    
    print_string("RESULT=");
    printu32(i2);   

    anydata[] ax2 = [];
	ax2[0] = 234;
	anydata x = ax2;
	anydata[] axTemp = <anydata[]>x;
	int i3 = <int>axTemp[0];
    print_string("RESULT=");
	printu32(i3);

    anydata[] ax3 = [];
    ax3[0] = "Test str";
    string s = <string>ax3[0];
    print_string("RESULT=");
    print_string(s);           
}
// CHECK: RESULT=23
// CHECK: RESULT=239
// CHECK: RESULT=234
// CHECK: RESULT=Test str
