// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printf32(float val) = external;

public function bar(float a, float b) returns float
{
    return b;
}

public function main() {
    float c = bar(7.5, 10.5);
    print_string("RESULT=");
    printf32(c);
}

// CHECK: RESULT=10.5
