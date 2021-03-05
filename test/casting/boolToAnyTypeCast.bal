// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function bar(any z) returns boolean
{
    return <boolean>z;
}

public function main() {
    boolean b = true;
    boolean a = bar(b);
    if (a) {
        print_str("RESULT=");
        printu32(1);
    }
}

// CHECK: RESULT=1
