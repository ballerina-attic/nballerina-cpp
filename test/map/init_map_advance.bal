// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s
public function main() {
    int dan = 70;
    string best = "sam";
    map<int> marks = {dan, [best]: 80};
}
// CHECK: length=2
