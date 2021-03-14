// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

boolean _bal_result = false;
public function bar(any z) returns boolean
{
    return <boolean>z;
}

public function main() {
    boolean b = true;
    _bal_result = bar(b);
}

// CHECK: RETVAL=1
