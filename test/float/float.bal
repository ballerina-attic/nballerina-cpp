// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printf32(float val) = external;

public function main() {
    print_string("RESULT=");
    printf32(2.2);
}

// CHECK: RESULT=2.2
