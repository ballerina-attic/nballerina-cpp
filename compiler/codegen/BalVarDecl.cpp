#include "BalVarDecl.h"

namespace nballerina {

VarDecl::VarDecl(std::string name) : varName(name) {}

VarDecl::VarDecl(TypeDecl *ty, std::string name, VarKind k)
    : type(ty), varName(name), kind(k) {}

VarDecl::VarDecl(TypeDecl *ty, std::string name, VarKind k, VarScope sc,
                 bool ignore)
    : type(ty), varName(name), kind(k), scope(sc), ignoreVariable(ignore) {}

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
