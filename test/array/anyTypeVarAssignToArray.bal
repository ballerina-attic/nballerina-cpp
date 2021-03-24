// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int[] a = [5];
    any x = 2;
    a[0] = <int>x;
    print_string("RESULT=");
    printu32(a[0]);
}

// CHECK: RESULT=2
