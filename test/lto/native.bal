// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen"

public function print_integer(int val) = external;

public function native_echo(int val) returns int {
    return val;
}

public function main() {
    int sum = 0;
    int i = 0;
    while (i < 10000000000) {
        sum += native_echo(i);
        i = i + 1;
    }
    print_integer(sum);
}
