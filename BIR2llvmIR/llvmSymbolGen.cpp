#include "BIR.h"

Symbol::Symbol() {}

Symbol::Symbol(int tagp, int flagsp, string namep,/* PackageID *pkgIDp,*/ SymbolKind *kindp,
	       TypeDecl *typep, Symbol *ownerp, Scope *scopep): tag(tagp), flags(flagsp),
		name(namep), kind(kindp), type(typep), owner(ownerp),
		  scope(scopep) {}
Symbol::~Symbol() {}

void Symbol::translate(LLVMModuleRef &modRef) { }

