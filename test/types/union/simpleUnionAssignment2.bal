// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

boolean _bal_result = false;

public function main() {
    int|boolean|string uTemp = true;
    int|boolean|string u = uTemp;
    boolean i = <boolean>u;
    _bal_result = i;
}
// CHECK: RETVAL=1
