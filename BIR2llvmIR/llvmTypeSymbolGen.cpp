#include "BIR.h"

TypeSymbol::TypeSymbol() {}

TypeSymbol::TypeSymbol(int symTag, int flags, string name, SymbolKind *kind,
               TypeDecl *type, Symbol *owner,  Scope *scope, bool isLab):
	Symbol(symTag, flags, name, kind, type, owner, scope), isLabel(isLab) {

}

TypeSymbol::~TypeSymbol() { }

