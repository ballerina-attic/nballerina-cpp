// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printf32(float val) = external;

float val = 7.7;

public function main() {
    val = 7.7;    // TODO remove ressaignment when global init is fixed
    print_string("RESULT=");
    printf32(val);
}

// CHECK: RESULT=7.7
