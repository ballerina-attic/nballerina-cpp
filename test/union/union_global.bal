// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function printu32(int val) = external;

int|string a = 24;

public function main() {    
    a = 24;
    int x = <int>a;
    print_str("RESULT=");
    printu32(x); 
}
// CHECK: RESULT=24
