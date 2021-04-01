// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function main() {
    string[][] arr1 = [[], []];
    arr1[0][1] = "Hello";
    arr1[1][2] = " World";

    string a = arr1[0][1];
    string b = arr1[1][2];
    print_string(a);
    print_string(b);
}
// CHECK: Hello World
