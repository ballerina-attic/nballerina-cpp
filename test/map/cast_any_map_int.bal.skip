// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int[] i = [3];
    i[2] = 10;
    map<int> marks = {sam: 50, jon: 60};
    any x = marks;
    map<int> marks2 = <map<int>>x;

    int? loadVal = marks2["jon"];
    int johnsMarks = <int>loadVal;
    print_string("RESULT=");
    print_integer(johnsMarks);

    int? loadVal2 = marks2["sam"];
    int samMarks = <int>loadVal2;
    print_string("RESULT=");
    print_integer(samMarks);
}
// CHECK: RESULT=60
// CHECK: RESULT=50
