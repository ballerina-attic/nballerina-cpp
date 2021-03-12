// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
    int[] a = [5];
    any x = 2;
    a[0] = <int>x;
    _bal_result = a[0];
}

// CHECK: RETVAL=2
