// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int|string var1 = 42;
    any result1 = <any>var1;
    print_string("RESULT=");
    print_integer(<int>result1);
}
// CHECK: RESULT=42
