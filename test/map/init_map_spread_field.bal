// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    map<int> boys = {Jake: 80, Steve: 90};
    map<int> girls = {Amy: 85, Sam: 88};
    map<int> allMarks = {...boys, ...girls};

    int? loadVal = allMarks["Jake"];
    int jakeMarks = <int>loadVal;
    print_string("RESULT=");
    printu32(jakeMarks);

    int? loadVal2 = allMarks["Sam"];
    int samMarks = <int>loadVal2;
    print_string("RESULT=");
    printu32(samMarks);

}
// CHECK: RESULT=80
// CHECK: RESULT=88
