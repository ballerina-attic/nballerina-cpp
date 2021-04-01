// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_boolean(boolean val) = external;

public function main() {
    boolean[][] arr1 = [[], []];
    arr1[0][1] = true;
    arr1[1][1] = false;
    arr1[3][3] = true;
    int index1 = 3;
    boolean b = arr1[index1][index1];
    if (b) {
        print_boolean(arr1[1][1]);
    }
}
// CHECK: false
