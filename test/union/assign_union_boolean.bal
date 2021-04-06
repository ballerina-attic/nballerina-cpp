// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

public function main() {
    int|boolean|string uTemp = true;
    int|boolean|string u = uTemp;
    boolean i = <boolean>u;
    if (i) {
        print_string("RESULT=");
        print_i64(1);
    }
}
// CHECK: RESULT=1
