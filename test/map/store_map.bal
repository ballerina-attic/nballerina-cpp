// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s
public function main() {
    map<int> marks = {};
    marks["jake"] = 80;
    string key = "jon";    // TODO remove ressaignment when global init is fixed
    int value = 60;    // TODO remove ressaignment when global init is fixed
    marks[key] = value;
}
// CHECK: length=2
