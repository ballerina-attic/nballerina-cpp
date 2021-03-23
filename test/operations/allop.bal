// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

int _bal_result = 0;

public function main() {
    int a = 10;
    int b = 5;
    int c = 0;
    int i = 0;
    boolean e = false;
    boolean f = false;
    c = a + b;
    c = a - b;
    c = a * b;
    c = a / b;
    c = a % b;
    int g = -a;
    e = !f;
    int k = ~a;
    if (a == 10) {
        _bal_result = _bal_result + 1;
    }
    if (a < b) {
        e = false;
        _bal_result = _bal_result + 1;
    }
    if (a > b) {
        f = true;
        _bal_result = _bal_result + 1;
    }
    if (a <= b) {
        e = true;
        _bal_result = _bal_result + 1;
    }
    if (a >= b) {
        f = false;
        _bal_result = _bal_result + 1;
    }
    else {
        f = true;
        _bal_result = _bal_result + 1;
    }
    while (i < 10) {
        i = i + 1;
        _bal_result = _bal_result + 1;
    }
    print_str("RESULT=");
    printu32(_bal_result);
}

// CHECK: RESULT=13
