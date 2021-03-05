// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function bar(any z) returns string
{
    string strl = <string>z;
    return strl;
}

public function main() {
    string b = "Hello World";
    string c = bar(b);
    printu32(1);
}

// CHECK: RETVAL=1
