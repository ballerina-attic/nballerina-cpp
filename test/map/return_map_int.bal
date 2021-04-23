// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

function foo() returns map<int> {
    return {sam: 50, jon: 60};
}

public function main() {
    map<int> marks = foo();

    int? loadVal = marks["jon"];
    int johnsMarks = <int>loadVal;
    print_string("RESULT=");
    print_integer(johnsMarks);

    int? loadVal2 = marks["sam"];
    int samMarks = <int>loadVal2;
    print_string("RESULT=");
    print_integer(samMarks);
}
// CHECK: RESULT=60
// CHECK: RESULT=50

