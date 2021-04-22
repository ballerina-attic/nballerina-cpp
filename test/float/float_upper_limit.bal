// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printf64(float val) = external;

public function main() {
    print_string("RESULT=");
    //Passing value greater than upper limit of float i.e. 3.402823466 E + 39
    float f = 3.402823465999999976608952504680408952584;
    printf64(f);
}

// CHECK: RESULT=3.402823466
