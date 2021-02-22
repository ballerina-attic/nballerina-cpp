#include "BalLocation.h"

namespace nballerina {
Location::Location(std::string name, int sline, int scol, int eline, int ecol)
    : fileName(name), sLine(sline), sCol(scol), eLine(eline), eCol(ecol) {}

std::string Location::getFileName() { return fileName; }
int Location::getStartLineNum() { return sLine; }
int Location::getStartColumnNum() { return sCol; }
int Location::getEndLineNum() { return eLine; }
int Location::getEndColumnNum() { return eCol; }
} // namespace nballerina