// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

function any_to_int1(any a) returns int {
    int i = <int>a;
    return i;
}

function int_to_any1(int i) returns any {
    any a = i;
    return a;
}

function test(int i) {
    int c = any_to_int1(int_to_any1(i));
    print_string("RESULT=");
    print_integer(c);
}

public function main() {
    test(12147483647);
    test(0);
    test(1);
    test(2);
    test(3);
    test(100);
    test(1024);
    test(-1);
    test(-2);
    test(-3);
    test(-4611686018427387906);
    test(-4611686018427387905);
    test(-4611686018427387904);
    test(4611686018427387902);
    test(4611686018427387903);
    test(4611686018427387904);
}
// CHECK: RESULT=12147483647
// CHECK: RESULT=0
// CHECK: RESULT=1
// CHECK: RESULT=2
// CHECK: RESULT=3
// CHECK: RESULT=100
// CHECK: RESULT=1024
// CHECK: RESULT=-1
// CHECK: RESULT=-2
// CHECK: RESULT=-3
// CHECK: RESULT=-4611686018427387906
// CHECK: RESULT=-4611686018427387905
// CHECK: RESULT=-4611686018427387904
// CHECK: RESULT=4611686018427387902
// CHECK: RESULT=4611686018427387903
// CHECK: RESULT=4611686018427387904
