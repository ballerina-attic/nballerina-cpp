// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s


public function print_string(string val) = external;

public function printu32(int val) = external;

map<int> marks = {};
int value = 60;
string key = "";

public function main() {
    marks = {};    // TODO remove ressaignment when global init is fixed
    key = "jon";    // TODO remove ressaignment when global init is fixed
    value = 60;    // TODO remove ressaignment when global init is fixed
    marks[key] = value;
    marks["jake"] = 80;

    int? loadVal = marks["jon"];
    int johnsMarks = <int>loadVal;
    print_string("RESULT=");
    printu32(johnsMarks);

    int? loadVal2 = marks["jake"];
    int jakeMarks = <int>loadVal2;
    print_string("RESULT=");
    printu32(jakeMarks);
}
// CHECK: RESULT=60
// CHECK: RESULT=80
