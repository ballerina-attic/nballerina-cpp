#include "BIR.h"

TypeDecl::TypeDecl() {}

TypeDecl::TypeDecl(TypeSymbol *tsymbol):typeSym(tsymbol) {
}

TypeDecl::TypeDecl(int tagp, TypeSymbol *tsymbol, int flagsp):
			tag(tagp), typeSym(tsymbol), flags(flagsp) { }

TypeDecl::TypeDecl(int tagp, TypeSymbol *tsymbol, string namep, int flagsp):
		    tag(tagp), typeSym(tsymbol), name(namep), flags(flagsp) { }

TypeDecl::~TypeDecl() { }

void TypeDecl::translate(LLVMModuleRef &modRef) { }
