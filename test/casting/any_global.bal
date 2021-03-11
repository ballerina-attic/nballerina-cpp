// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
any a = 23;

public function main() {    
    a = 23;
    int x = <int>a;
    _bal_result = x;
}
// CHECK: RETVAL=23
