// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

map<int> marks = {};
int value = 60;
string key = "";

public function main() {
    marks = {};
    key = "jon";
    value = 60;
    marks[key] = value;
}
// CHECK: length=1
