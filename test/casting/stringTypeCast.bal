// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function bar(any z) returns string
{
    string strl = <string>z;
    return strl;
}

public function main() {
    string b = "Hello World";
    string c = bar(b);
    print_str("RESULT=");
    print_str(c);
}

// CHECK: RESULT=Hello World
