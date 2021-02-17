#include "BIR.h"

TypeDecl::TypeDecl(int tagp, int flagsp) : tag(tagp), flags(flagsp) {}

TypeDecl::TypeDecl(int tagp, string namep, int flagsp)
    : tag(tagp), name(namep), flags(flagsp) {}

void TypeDecl::translate(__attribute__((unused)) LLVMModuleRef &modRef) {}
