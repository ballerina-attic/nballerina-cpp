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
    void deserialize();

  private:
    BIRFunction readFunction();
    BasicBlock readBasicBlock();
    NonTerminatingInsn readNonTerminatingInsn();
    TerminatingInsn readTerminatingInsn();
    VarDecl readVarDecl();
    Param readParam();
    Symbol readSymbol();
};

#endif // BIRREADER_H
