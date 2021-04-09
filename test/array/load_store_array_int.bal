// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int[] arr1 = [5];
    arr1[0] = 1;
    arr1[1] = 5;
    arr1[3] = 10;
    int index1 = 3;

    int[] arr2 = [6];
    arr2[1] = 10;
    arr2[3] = 50;
    arr2[4] = 100;
    int index2 = 4;

    int a = arr1[0] + arr1[1] + arr1[index1];
    int b = arr2[1] + arr2[3] + arr2[index2];
    print_string("RESULT=");
    print_integer(a + b);
}
// CHECK: RESULT=176
