// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
int|string a = 24;

public function main() {    
    a = 24;
    int x = <int>a;
    _bal_result = x;
}
// CHECK: RETVAL=24
