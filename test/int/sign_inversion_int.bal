// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int a = 10;
    int g = -a;
    print_string("RESULT=");
    print_integer(g);
    int b = -23;
    int h = -b;
    print_string("RESULT=");
    print_integer(h);
}

// CHECK: RESULT=-10
// CHECK: RESULT=23
