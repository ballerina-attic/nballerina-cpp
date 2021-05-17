// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    map<anydata> marks = {jon: 60};

    anydata? loadVal = marks["jon"];
    int johnsMarks = <int>loadVal;
    print_string("RESULT=");
    print_integer(johnsMarks);
}
// CHECK: RESULT=60
