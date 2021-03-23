// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int[] arr1 = [5];
    arr1[0] = 1;
    arr1[1] = 5;
    arr1[3] = 10;
    int a = arr1[0] + arr1[1] + arr1[3];
    print_str("RESULT=");
    printu32(a);
}
// CHECK: RESULT=16
