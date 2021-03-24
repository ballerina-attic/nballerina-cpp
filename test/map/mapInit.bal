// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s
public function main() {
    map<int> marks = {sam: 50, jon: 60};
}
// CHECK: length=2
