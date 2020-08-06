#ifndef BIRREADER_H
#define BIRREADER_H

#include <istream>
#include <string>
#include <vector>
#include "bir.h"

use namespace std;
use std::string;
use std::vec;


class BIRReader {
  private:
  istream *is;
  string   filename;

  public:
    BIRReader();
    BIRReader(string name); // This should open the file and initialize the
                            // istream
    ~BIRReader();

    string name()  { return filename; }
    char readByte();
    void deserialize(istream *dataInStream) {

        // Read import packages.
        readPackages(dataInStream);

    }

    void readPackages(istream *dataInStream) {

    }

  private:
    BIRFunction readFunction();
    BasicBlock readBasicBlock();
    NonTerminatingInsn readNonTerminatingInsn();
    TerminatingInsn readTerminatingInsn();
    VarDecl readVarDecl();
    Param readParam();
    Symbol readSymbol();
};

struct BinBIRPackage {
    string org;
    string name;
    string version;
    string sourceFileName;

    vector<BIRFunction> functions;
    friend istream &operator >> (istream &s, struct BinBIRPackage &readBIRPackage) {
        is >> readBIRPackage.org >> readBIRPackage.name >> readBIRPackage.version;
        return is;
    }
};

#endif // BIRREADER_H
