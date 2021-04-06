// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    any var1 = 42;
    int result1 = <int>var1;
    int|string var2 = 7;
    int result2 = <int>var2;
    print_string("RESULT=");
    print_integer(result1 + result2);
}
// CHECK: RESULT=49
