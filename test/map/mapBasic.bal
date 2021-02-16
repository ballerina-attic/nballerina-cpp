// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s
int _bal_result = 0;
public function main() {
    map<int> marks = {};
    marks["jon"] = 60;
}
// CHECK: length=1
