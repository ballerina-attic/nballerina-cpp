// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function printf64(float val) = external;

public function bar(any z) returns float
{
    float fl = <float>z;
    return fl;
}

public function main() {
    float b = 10.5;
    float c = bar(b);
    printf64(c);
}

// CHECK: RETVAL=40
