// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int? i = 23;
    int j = <int>i;
    boolean? b = true;
    if (<boolean>b) {
        j = j + 1;
    }
    print_string("RESULT=");
    print_integer(j);
}

// CHECK: RESULT=24
