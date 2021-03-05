// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printu32(int val) = external;

public function main() {
    () nilVal = bar();
    foo();
    () n = baz();
    if (n == ()) {
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

// CHECK: RETVAL=1
