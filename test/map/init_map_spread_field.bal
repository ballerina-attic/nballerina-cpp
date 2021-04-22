// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    map<int> boys = {Jake: 80, Steve: 90};
    map<int> girls = {Amy: 85, Sam: 88};
    map<int> allMarks = {...boys, ...girls};

    int? loadVal = allMarks["Jake"];
    int jakeMarks = <int>loadVal;
    print_string("RESULT=");
    print_integer(jakeMarks);

    int? loadVal2 = allMarks["Sam"];
    int samMarks = <int>loadVal2;
    print_string("RESULT=");
    print_integer(samMarks);

}
// CHECK: RESULT=80
// CHECK: RESULT=88
