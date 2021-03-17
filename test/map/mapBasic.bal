// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s
int _bal_result = 0;
public function main() {
    map<int> marks = {};
    marks["jon"] = 60;
}
// CHECK: length=1
