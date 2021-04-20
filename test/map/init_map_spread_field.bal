// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    map<int> boys = {Jake: 80, Steve: 90};
    map<int> allMarks = {...boys};

    int? loadVal = allMarks["Jake"];
    int jakeMarks = <int>loadVal;
    print_string("RESULT=");
    print_integer(jakeMarks);

}
// CHECK: RESULT=80