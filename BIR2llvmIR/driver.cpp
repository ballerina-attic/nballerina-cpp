#include "BIR.h"
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
using namespace std;

int main()
{
  Location *loc1 = new Location("newFile",2,9);
  VarDecl *vDecllhs = new VarDecl(loc1, "1stVarLhs", "1metanameLhs");
  TypeDecl *tyDecl11 = new TypeDecl();
  tyDecl11->setTypeDeclName ("int");
  vDecllhs->setTypeDecl(tyDecl11);

  Operand *lOp = new Operand(vDecllhs);
  VarDecl *vDeclrhs = new VarDecl(loc1, "1stVarRhs", "tmetanameRhs");
  TypeDecl *tyDecl2 = new TypeDecl();
  tyDecl2->setTypeDeclName ("int");
  vDeclrhs->setTypeDecl(tyDecl2);  

  Operand *rOp = new Operand(vDeclrhs);
  MoveInsn *moveInsn = new MoveInsn(loc1, INS_KIND_MOVE, lOp, rOp);
  NonTerminatorInsn *NonTInsn = static_cast<NonTerminatorInsn*>(moveInsn);
  BasicBlockT *bb = new BasicBlockT("BB1");
  bb->addNonTermInsn(NonTInsn);
  TypeDecl *tyDecl = new TypeDecl();
  tyDecl->setTypeDeclName ("void");
  InvokableType *invokT = new InvokableType();
  invokT->setReturnType(tyDecl);
  
  BIRFunction *func = new BIRFunction(loc1, "myFunc", 10, invokT, "MyWorker");
  func->setBasicBlockT(bb);
  func->setLocalVar(vDecllhs);
  func->setLocalVar(vDeclrhs);
  VarDecl *retVar = new VarDecl(loc1, "void", "metaName");
  TypeDecl *tyDecl1 = new TypeDecl();
  tyDecl1->setTypeDeclName ("void");
  retVar->setTypeDecl(tyDecl1);
  func->setReturnVar(retVar);
  BIRPackage *pkg = new BIRPackage ("$anon", ".", "0.0.0", "sourceFile");
  pkg->addFunction(func);
  pkg->translate();
}
