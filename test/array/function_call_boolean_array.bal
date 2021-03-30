// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function test(boolean[] a) returns boolean {
    boolean c = a[1];
    return c;
}
public function main() {
    boolean[4] arr1 = [];
    arr1[0] = true;
    arr1[1] = true;
    boolean b = arr1[0];
    if (test(arr1)) {
        print_string("RESULT=");
    }
    if (b) {
        printu32(1);
    }
}

// CHECK: RESULT=1
