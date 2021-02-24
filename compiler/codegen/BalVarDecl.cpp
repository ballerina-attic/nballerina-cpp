#include "BalVarDecl.h"

namespace nballerina {

VarDecl::VarDecl(Location *loc, std::string pname, std::string pmetaVarName)
    : varName(pname), metaVarName(pmetaVarName) {
  setLocation(loc);
}

VarDecl::VarDecl(Location *loc, TypeDecl *ty, std::string name,
                 std::string metaName, VarKind k, VarScope sc, int offset)
    : type(ty), varName(name), metaVarName(metaName), kind(k), scope(sc),
      insOffset(offset) {
  setLocation(loc);
}

TypeDecl *VarDecl::getTypeDecl() { return type; }
VarKind VarDecl::getVarKind() { return kind; }
VarScope VarDecl::getVarScope() { return scope; }
BasicBlock *VarDecl::getStartBB() { return startBB; }
BasicBlock *VarDecl::getEndBB() { return endBB; }
std::string VarDecl::getVarName() { return varName; }
std::string VarDecl::getMetaVarName() { return metaVarName; }
int VarDecl::getInsOffset() { return insOffset; }
bool VarDecl::ignore() { return ignoreVariable; }

void VarDecl::setTypeDecl(TypeDecl *newType) { type = newType; }
void VarDecl::setVarKind(VarKind newKind) { kind = newKind; }
void VarDecl::setVarScope(VarScope newScope) { scope = newScope; }
void VarDecl::setStartBB(BasicBlock *bb) { startBB = bb; }
void VarDecl::setEndBB(BasicBlock *bb) { endBB = bb; }
void VarDecl::setVarName(std::string newName) { varName = newName; }
void VarDecl::setMetaVarName(std::string newName) { metaVarName = newName; }
void VarDecl::setInsOffset(int offset) { insOffset = offset; }
void VarDecl::setIgnore(bool truth) { ignoreVariable = truth; }

} // namespace nballerina
