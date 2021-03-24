// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    int[]? arrOrNil = [];
    int[] arr = <int[]>arrOrNil;
    arr[0] = 32;
    print_str("RESULT=");
    printu32(arr[0]);
}
// CHECK: RESULT=32
