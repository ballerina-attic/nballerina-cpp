// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    () nilVal = bar();
    foo();
    () n = baz();
    if (n == ()) {
        print_str("RESULT=");
        printu32(1);
    }
}

function foo() {
    () nilVal = ();
}

function bar() returns () {
    return ();
}

function baz() returns () {
    () nilVal = ();
    return nilVal;
}

// CHECK: RESULT=1
