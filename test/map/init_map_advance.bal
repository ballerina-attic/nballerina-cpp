// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int Dan = 70;
    string best = "Sam";
    map<int> marks = {Dan, [best]: 80};

    int? loadVal = marks["Dan"];
    int danMarks = <int>loadVal;
    print_string("RESULT=");
    printu32(danMarks);

    int? loadVal2 = marks["Sam"];
    int samMarks = <int>loadVal2;
    print_string("RESULT=");
    printu32(samMarks);

}
// CHECK: RESULT=70
// CHECK: RESULT=80
