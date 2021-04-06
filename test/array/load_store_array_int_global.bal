// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

int index = 3;
int[] arr1 = [5];
public function main() {
    arr1 = [5];    // TODO remove ressaignment when global init is fixed
    index = 3;    // TODO remove ressaignment when global init is fixed
    arr1[0] = 1;
    arr1[1] = 5;
    arr1[3] = 10;
    int a = arr1[0] + arr1[1] + arr1[index];
    print_string("RESULT=");
    print_i64(a);
}
// CHECK: RESULT=16
