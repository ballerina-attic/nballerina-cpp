// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function main() {
    map<int>? marksOrNil = {};
    map<int> marks = <map<int>>marksOrNil;
    marks["jon"] = 60;
}
// CHECK: length=1
