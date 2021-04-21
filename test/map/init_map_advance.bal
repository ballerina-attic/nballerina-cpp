// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int Dan = 70;
    string best = "Sam";
    map<int> marks = {Dan, [best]: 80};

    int? loadVal = marks["Dan"];
    int danMarks = <int>loadVal;
    print_string("RESULT=");
    print_integer(danMarks);

    int? loadVal2 = marks["Sam"];
    int samMarks = <int>loadVal2;
    print_string("RESULT=");
    print_integer(samMarks);

}
// CHECK: RESULT=70
// CHECK: RESULT=80
