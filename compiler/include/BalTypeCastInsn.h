#ifndef __BALTYPECASTINSN__H__
#define __BALTYPECASTINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declare
class TypeDecl;
class Operand;

class TypeCastInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  TypeDecl *typeDecl;
  bool checkTypes;

public:
  TypeCastInsn() = default;
  TypeCastInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp,
               TypeDecl *tDecl, bool checkTypes);
  ~TypeCastInsn() = default;

  Operand *getRhsOp();
  TypeDecl *getTypeDecl();
  bool mustCheckTypes();

  void setRhsOp(Operand *op);
  void setTypeDecl(TypeDecl *tDecl);
  void setTypesChecking(bool checktypes);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALTYPECASTINSN__H__