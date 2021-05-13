// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function addx(int a, int b) returns int= external;

public function main() {
    int c= addx(10,5);
    print_string("RESULT=");
    print_integer(c);
}

// CHECK: RESULT=15

