#ifndef __BALVARDECL__H__
#define __BALVARDECL__H__

#include "BalEnums.h"
#include "interfaces/PackageNode.h"
#include <string>

namespace nballerina {

// Forward Declaration
class BasicBlock;
class TypeDecl;

class VarDecl : public PackageNode {
private:
  TypeDecl *type;
  std::string varName;
  std::string metaVarName;
  VarKind kind;
  VarScope scope;
  bool ignoreVariable;
  BasicBlock *endBB;
  BasicBlock *startBB;
  int insOffset;

public:
  VarDecl() = delete;
  // For global vars, local  ,arg, return
  VarDecl(TypeDecl *ty, std::string name, VarKind k);
  // No type - for function params
  VarDecl(std::string name);
  // Read operad - typedcl could be null
  VarDecl(TypeDecl *ty, std::string name, VarKind k, VarScope sc, bool ignore);
  virtual ~VarDecl() = default;

  TypeDecl *getTypeDecl();
  VarKind getVarKind();
  VarScope getVarScope();
  BasicBlock *getStartBB();
  BasicBlock *getEndBB();
  std::string getVarName();
  std::string getMetaVarName();
  int getInsOffset();
  bool ignore();

  void setTypeDecl(TypeDecl *newType);
  void setVarKind(VarKind newKind);
  void setVarScope(VarScope newScope);
  void setStartBB(BasicBlock *bb);
  void setEndBB(BasicBlock *bb);
  void setVarName(std::string newName);
  void setMetaVarName(std::string newName);
  void setInsOffset(int offset);
  void setIgnore(bool truth);
};

} // namespace nballerina

#endif //!__BALVARDECL__H__