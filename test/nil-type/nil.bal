// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function main() {
  () nilVal = bar();
  foo();
  () n = baz();
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

// CHECK: RETVAL=0
