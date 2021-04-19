// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    map<int> marks = {};
    marks["jake"] = 80;
    string key = "jon";
    int value = 60;
    marks[key] = value;

    int? loadVal = marks["jon"];
    int johnsMarks = <int>loadVal;
    print_string("RESULT=");
    print_integer(johnsMarks);

    int? loadVal2 = marks["jake"];
    int jakeMarks = <int>loadVal2;
    print_string("RESULT=");
    print_integer(jakeMarks);
}
// CHECK: RESULT=60
// CHECK: RESULT=80
