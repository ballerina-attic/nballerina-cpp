// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function bar(any z) returns boolean
{
    return <boolean>z;
}

public function main() {
    boolean b = true;
    boolean a = bar(b);
    if (a) {
        print_string("RESULT=");
        printu32(1);
    }
}

// CHECK: RESULT=1
