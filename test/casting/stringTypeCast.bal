// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function bar(any z) returns string
{
    string strl = <string>z;
    return strl;
}

public function main() {
    string b = "Hello World";
    string c = bar(b);
    print_str(c);
}

// CHECK: RETVAL=Hello World
