// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function printf32(float val) = external;

public function main() {
    printf32(2.2);
}

// CHECK: RESULT=2.2
