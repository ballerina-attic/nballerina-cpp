// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function bar(map<int> container, string key) returns int? {
    return container[key];
}

public function main() {
    map<int> marks = {sam: 50, jon: 60};

    int? loadVal = bar(marks, "jon");
    int johnsMarks = <int>loadVal;
    print_string("RESULT=");
    printu32(johnsMarks);

    int? loadVal2 = bar(marks, "sam");
    int samMarks = <int>loadVal2;
    print_string("RESULT=");
    printu32(samMarks);
}
// CHECK: RESULT=60
// CHECK: RESULT=50
