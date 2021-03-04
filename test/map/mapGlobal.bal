// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;

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