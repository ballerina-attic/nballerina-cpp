#include "BIR.h"

TypeDecl::TypeDecl() {}

TypeDecl::TypeDecl(int tagp, int flagsp):
			tag(tagp), flags(flagsp) {}

TypeDecl::TypeDecl(int tagp, string namep, int flagsp):
		    tag(tagp), name(namep), flags(flagsp) {}

TypeDecl::~TypeDecl() {}

void TypeDecl::translate(LLVMModuleRef &modRef) {}
