// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s
int _bal_result = 0;
public function main() {
    int dan = 70;
    string best = "sam";
    map<int> marks = {dan, [best]: 80};
    _bal_result = 0;
}
// CHECK: length=2
