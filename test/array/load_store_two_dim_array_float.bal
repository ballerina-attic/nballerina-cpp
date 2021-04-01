// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    float[2][3] arr1 = [[], []];
    arr1[0][1] = 1.45;
    arr1[1][1] = 5.5;
    print_string("RESULT=");
    printu32(123);// TODO: We have to print float after fix the float print issue.
}
// CHECK: RESULT=123
