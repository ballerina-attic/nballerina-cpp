// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_byte(byte val) = external;

byte[] arr1 = [5];
int index1 = 3;

public function main() {
    arr1 = [5];    // TODO remove ressaignment when global init is fixed
    index1 = 3;    // TODO remove ressaignment when global init is fixed
    arr1[0] = 1;
    arr1[1] = 5;
    arr1[3] = 10;

    byte a = arr1[index1];
    print_string("RESULT=");
    print_byte(a);
    print_string("RESULT=");
    print_byte(arr1[0]);
    print_string("RESULT=");
    print_byte(arr1[2]);
}
// CHECK: RESULT=10
// CHECK: RESULT=1
// CHECK: RESULT=0
