// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

int? a = 4;

public function main() {
    a = 24;
    print_string("RESULT=");
    printu32(<int>a);
}
// CHECK: RESULT=24
