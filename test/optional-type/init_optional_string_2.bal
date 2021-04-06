// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

public function main() {
    string? x = ();
    () n = <()>x;
    print_string("RESULT=");
    print_i64(231);
}

// CHECK: RESULT=231
