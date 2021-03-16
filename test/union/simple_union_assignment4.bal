// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    any var1 = 42;
    int result1 = <int>var1;
    int | string var2 = 7;
    int result2 = <int>var2;
    print_str("RESULT=");
    printu32(result1 + result2); 
}
// CHECK: RESULT=49
