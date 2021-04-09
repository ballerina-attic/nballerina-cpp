// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    print_string("RESULT=");
    printf64(2.2);
}

// CHECK: RESULT=2.2
