// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

map<int> marks = {};
int value = 60;
string key = "";

public function main() {
    marks = {};    // TODO remove ressaignment when global init is fixed
    key = "jon";    // TODO remove ressaignment when global init is fixed
    value = 60;    // TODO remove ressaignment when global init is fixed
    marks[key] = value;
}
// CHECK: length=1
