// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

public function main() {
    int|string var1 = 42;
    any result1 = <any>var1;
    print_string("RESULT=");
    printu64(<int>result1);
}
// CHECK: RESULT=42
