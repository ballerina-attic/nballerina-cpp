// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s
public function main() {
    map<int> marks = {};
    marks["jon"] = 60;
}
// CHECK: length=1
