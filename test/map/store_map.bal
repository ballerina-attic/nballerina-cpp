// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s
public function main() {
    map<int> marks = {};
    marks["jake"] = 80;
    string key = "jon";
    int value = 60;
    marks[key] = value;
}
// CHECK: length=2
