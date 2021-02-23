#include "BalNode.h"

namespace nballerina {
BIRNode::BIRNode(Location *pos) { setLocation(pos); }

BIRNode::BIRNode(BIRPackage *pkgptr) : pkg(pkgptr) {}

BIRNode::BIRNode(Location *pos, BIRPackage *pkgptr) : pkg(pkgptr) {
  setLocation(pos);
}

void BIRNode::translate(__attribute__((unused)) LLVMModuleRef &modRef) {}
BIRPackage *BIRNode::getPkgAddress() { return pkg; }
void BIRNode::setPkgAddress(BIRPackage *pkgptr) { pkg = pkgptr; }

} // namespace nballerina
