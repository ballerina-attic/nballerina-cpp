// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

map<anydata> marks1 = {};
int value = 34;
string key = "";

public function main() {
    map<anydata> marks2 = {sam: 50, jon: 60};
    marks2["edd"] = 70;

    marks1 = {}; // TODO remove ressaignment when global init is fixed
    key = "jon"; // TODO remove ressaignment when global init is fixed
    value = 64; // TODO remove ressaignment when global init is fixed
    marks1[key] = value;

    int dan = 70;
    string best = "sam";
    map<anydata> marks3 = {dan, [best] : 80};
}
// CHECK: length=2
// CHECK: length=3
// CHECK: length=1
// CHECK: length=2
