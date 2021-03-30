// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int? i = 23;
    int j = <int>i;
    boolean? b = true;
    if (<boolean>b) {
        j = j + 1;
    }
    print_string("RESULT=");
    printu32(j);
}

// CHECK: RESULT=24
