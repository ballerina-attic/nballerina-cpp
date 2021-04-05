// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu64(int val) = external;

public function main() {
    string? x = ();
    () n = <()>x;
    print_string("RESULT=");
    printu64(231);
}

// CHECK: RESULT=231
