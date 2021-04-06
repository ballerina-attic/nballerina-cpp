// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

public function main() {
    int? i = 23;
    int j = <int>i;
    boolean? b = true;
    if (<boolean>b) {
        j = j + 1;
    }
    print_string("RESULT=");
    print_i64(j);
}

// CHECK: RESULT=24
