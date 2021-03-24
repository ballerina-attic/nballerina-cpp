// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int[] arr1 = [5];
    int[] arr2 = [6];
    print_string("RESULT=");
    printu32(1);
}

// CHECK: RESULT=1
