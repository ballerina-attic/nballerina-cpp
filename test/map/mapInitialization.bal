// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s
int _bal_result = 0;
public function main() {
    map<int> marks = {sam: 50, jon: 60};
    _bal_result = 0;
}
// CHECK: length=2
