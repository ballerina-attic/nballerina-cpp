// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function print_boolean(boolean val) = external;
public function main() {
    float a = 10.5;
    float b = 5.5;
    print_string("RESULT=");
    if (a == b) {
        printf64(a);
    }
    else {
        printf64(b);
    }
    float c = -10.5;
    float d = -10.5;
    print_string("RESULT=");
    if (c == d) {
        printf64(c);
    }
    else {
        printf64(0.0);
    }
    
    float k = 5.5;
    float l = 5.5;
    print_string("RESULT=");
    print_boolean(k >= l);

    print_string("RESULT=");
    print_boolean(k != l);
}
//CHECK: RESULT=5.5
//CHECK: RESULT=-10.5
//CHECK: RESULT=true
//CHECK: RESULT=false
