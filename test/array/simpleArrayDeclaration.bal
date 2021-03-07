// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public
function main() {
    int[] arr1 = [5];
    _bal_result = 1;
}

// CHECK: RETVAL=1
