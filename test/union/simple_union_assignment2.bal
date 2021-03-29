// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int|boolean|string uTemp = true;
    int|boolean|string u = uTemp;
    boolean i = <boolean>u;
    if (i) {
        print_string("RESULT=");
        printu32(1);
    }
}
// CHECK: RESULT=1
