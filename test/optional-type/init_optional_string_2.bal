// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    string? x = ();
    () n = <()>x;
    print_string("RESULT=");
    printu32(231);
}

// CHECK: RESULT=231
