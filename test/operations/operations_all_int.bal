// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int _bal_result = 0;

    int a = 10;
    int b = 5;
    int c = 0;
    int i = 0;
    boolean e = false;
    boolean f = false;
    c = a + b;        // 15
    c = c + a - b;    // 20
    c = c + a * b;    // 70
    c = c + a / b;    // 72
    c = c + a % b;    // 72
    int g = -a;
    c = c + g;        // 62
    e = !f;
    int k = ~a;
    c = c + k;        // 51
    if (a == 10) {
        _bal_result = _bal_result + 1;    // 1
    }
    if (a < b) {
        _bal_result = _bal_result - 1;
    }
    if (a > b) {
        _bal_result = _bal_result + 1;    // 2
    }
    if (a <= b) {
        _bal_result = _bal_result - 1;
    }
    if (a >= b) {
        _bal_result = _bal_result + 1;    // 3
    }
    else {
        _bal_result = _bal_result - 1;
    }
    if (f) {
        _bal_result = _bal_result - 1;
    }
    if (e) {
        _bal_result = _bal_result + 1;    // 4
    }
    while (i < 10) {
        i = i + 1;
        _bal_result = _bal_result + 1;
    }                                     // 14
    _bal_result = _bal_result + c;        // 65
    print_string("RESULT=");
    print_integer(_bal_result);
}

// CHECK: RESULT=65
