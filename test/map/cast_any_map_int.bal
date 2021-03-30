// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int[] i = [3];
    i[2] = 10;
    map<int> marks = {sam: 50, jon: 60};
    any x = marks;
    map<int> marks2 = <map<int>>x;

    int? loadVal = marks2["jon"];
    int johnsMarks = <int>loadVal;
    print_string("RESULT=");
    printu32(johnsMarks);

    int? loadVal2 = marks2["sam"];
    int samMarks = <int>loadVal2;
    print_string("RESULT=");
    printu32(samMarks);
}
// CHECK: RESULT=60
// CHECK: RESULT=50
