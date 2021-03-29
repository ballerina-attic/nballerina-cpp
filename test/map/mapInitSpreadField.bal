// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s
public function main() {
    map<int> boys = {Jake: 80, Steve: 90};
    map<int> girls = {Amy: 85, Sam: 88};
    map<int> allMarks = {...boys, ...girls};
}
// CHECK: length=4
