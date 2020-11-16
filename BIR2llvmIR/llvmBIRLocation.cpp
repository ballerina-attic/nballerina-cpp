#include "BIR.h"

Location::Location () {}

Location::Location(string name, int sline, int scol, int eline, int ecol):
		   fileName(name), sLine(sline), sCol(scol), eLine(eline), eCol(ecol) {}

void Location::translate (LLVMModuleRef &modRef) {}

Location::~Location () {}
