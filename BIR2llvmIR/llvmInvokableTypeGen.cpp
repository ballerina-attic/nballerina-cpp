#include "BIR.h"

InvokableType::InvokableType() {}

InvokableType::InvokableType(vector<TypeDecl*> paramTy, TypeDecl *restTy, TypeDecl *retTy)
                  /*TypeSymbol *tSymbol)*/:paramTypes(paramTy),restType(restTy),
			returnType(retTy)/*,TypeDecl(tSymbol)*/ {}

InvokableType::InvokableType(vector<TypeDecl*> paramTy, TypeDecl *retTy)
		 /*TypeSymbol *tSymbol)*/:paramTypes(paramTy),returnType(retTy)
			/*TypeDecl(tSymbol)*/ {}

InvokableType::~InvokableType() {}

void InvokableType::translate(LLVMModuleRef &modRef) {

}
