// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printf32(float val) = external;

public function foo(float a) returns float
{
    return a;
}

public function bar(float a, float b) returns float
{
    return b;
}

public function main() {
    float a = 10.5;
    float b = foo(a);
    float c = bar(b, 10.5);
    print_string("RESULT=");
    printf32(c);
}

// CHECK: RESULT=10.5
