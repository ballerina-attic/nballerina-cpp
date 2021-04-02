// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function main() {
    map<int> boys = {Jake: 80, Steve: 90};
    map<int> girls = {Amy: 85, Sam: 88};
    map<anydata> marks4 = {...boys, ...girls};
}
// CHECK: length=4
