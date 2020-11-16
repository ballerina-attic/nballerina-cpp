#include "BIR.h"

InvokableType::InvokableType() {}

InvokableType::InvokableType(vector<TypeDecl*> paramTy, TypeDecl *restTy, TypeDecl *retTy)
                  :paramTypes(paramTy),restType(restTy),
			returnType(retTy) {}

InvokableType::InvokableType(vector<TypeDecl*> paramTy, TypeDecl *retTy)
		 :paramTypes(paramTy),returnType(retTy) {}

InvokableType::~InvokableType() {}

void InvokableType::translate(LLVMModuleRef &modRef) {}
