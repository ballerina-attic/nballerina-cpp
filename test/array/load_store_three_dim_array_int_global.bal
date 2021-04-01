// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

int[][][] arr1 = [[], [], []];
int[][][] arr2 = [[], [], []];
public function main() {
    arr1 = [[], [], []];
    arr1[0][1][1] = 1;
    arr1[1][1][2] = 5;
    arr1[3][3][3] = 10;
    int index1 = 3;

    arr2 = [[], [], []];
    arr2[1][2][2] = 10;
    arr2[3][3][3] = 50;
    arr2[4][2][1] = 100;
    int index2 = 4;

    int a = arr1[0][1][1] + arr1[1][1][2] + arr1[index1][index1][index1];
    int b = arr2[1][2][2] + arr2[3][3][3] + arr2[index2][2][1];
    print_string("RESULT=");
    printu32(a + b);
}
// CHECK: RESULT=176
