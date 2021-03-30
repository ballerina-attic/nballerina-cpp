// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function test(string[] a) returns string {
    string c = a[1];
    return c;
}
public function main() {
    string[4] arr1 = [];
    arr1[0] = "Hello";
    arr1[1] = "World";
    print_string("RESULT=");
    print_string(test(arr1));
}

// CHECK: RESULT=World
