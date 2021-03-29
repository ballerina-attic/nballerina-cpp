// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    print_string("RESULT=");
    printu32(a + b);
}

// CHECK: RESULT=15

