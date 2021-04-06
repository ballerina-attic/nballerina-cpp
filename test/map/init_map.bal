// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i32(int val) = external;

public function main() {
    map<int> marks = {sam: 50, jon: 60};

    int? loadVal = marks["jon"];
    int johnsMarks = <int>loadVal;
    print_string("RESULT=");
    print_i32(johnsMarks);

    int? loadVal2 = marks["sam"];
    int samMarks = <int>loadVal2;
    print_string("RESULT=");
    print_i32(samMarks);
}
// CHECK: RESULT=60
// CHECK: RESULT=50
