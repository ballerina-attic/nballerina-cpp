#ifndef __BALVARDECL__H__
#define __BALVARDECL__H__

#include "BalEnums.h"
#include "Debuggable.h"
#include "PackageNode.h"
#include <string>

namespace nballerina {

// forward declare
class BIRBasicBlock;
class TypeDecl;

class VarDecl : public PackageNode, public Debuggable {
private:
  TypeDecl *type;
  std::string varName;
  std::string metaVarName;
  VarKind kind;
  VarScope scope;
  bool ignoreVariable;
  BIRBasicBlock *endBB;
  BIRBasicBlock *startBB;
  int insOffset;

public:
  VarDecl() = default;
  VarDecl(Location *pos, std::string name, std::string metaName);
  VarDecl(Location *pos, TypeDecl *ty, std::string name, std::string metaName,
          VarKind k, VarScope sc, int offset);
  virtual ~VarDecl() = default;

  TypeDecl *getTypeDecl();
  VarKind getVarKind();
  VarScope getVarScope();
  BIRBasicBlock *getStartBB();
  BIRBasicBlock *getEndBB();
  std::string getVarName();
  std::string getMetaVarName();
  int getInsOffset();
  bool ignore();

  void setTypeDecl(TypeDecl *newType);
  void setVarKind(VarKind newKind);
  void setVarScope(VarScope newScope);
  void setStartBB(BIRBasicBlock *bb);
  void setEndBB(BIRBasicBlock *bb);
  void setVarName(std::string newName);
  void setMetaVarName(std::string newName);
  void setInsOffset(int offset);
  void setIgnore(bool truth);
};
} // namespace nballerina

#endif //!__BALVARDECL__H__