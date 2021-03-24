// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

public function main() {
    string? x = ();
    () n = <()>x;
    print_str("RESULT=");
    printu32(231);
}

// CHECK: RESULT=231
