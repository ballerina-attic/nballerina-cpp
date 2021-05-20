// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    string? x = ();
    () n = <()>x;
    print_string("RESULT=");
    print_integer(231);
}

// CHECK: RESULT=231
