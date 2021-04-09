// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

map<anydata> marks1 = {};
int value = 34;
string key = "";

public function main() {
    map<anydata> marks2 = {sam: 50, jon: 60};
    marks2["edd"] = 70;
    anydata? i1 = marks2["edd"];

    marks1 = {}; // TODO remove ressaignment when global init is fixed
    key = "jon"; // TODO remove ressaignment when global init is fixed
    value = 64; // TODO remove ressaignment when global init is fixed
    marks1[key] = value;
    anydata? i2 = marks1[key];

    int dan = 70;
    string best = "sam";
    map<anydata> marks3 = {dan, [best] : 80};

    anydata? i3 = marks3[best];
    print_string("RESULT=");
    print_integer(<int>i1 + <int>i2 + <int>i3);
}
// CHECK: RESULT=214
