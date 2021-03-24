// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

int? a = 24;

public function main() {
    a = 24;
    print_str("RESULT=");
    printu32(<int>a);
}
// CHECK: RESULT=24
