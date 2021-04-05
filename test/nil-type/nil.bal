// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

public function main() {
    () nilVal = bar();
    foo();
    () n = baz();
    if (n == ()) {
        print_string("RESULT=");
        printu64(1);
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
