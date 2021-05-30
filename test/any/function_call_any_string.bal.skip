// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function bar(any z) returns string {
    string strl = <string>z;
    return strl;
}

public function main() {
    string b = "Hello World";
    string c = bar(b);
    print_string("RESULT=");
    print_string(c);
}

// CHECK: RESULT=Hello World
