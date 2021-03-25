// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

int g = 0;
any an = 0;
int a = 0;
public function bar(any l) returns int
{
    an = l;
    g = <int>an;
    return g;
}

public function main() {
    a = 35;                 // TODO remove ressaignment when global init is fixed
    print_string("RESULT=");
    printu32(bar(a));
}
// CHECK: RESULT=35
