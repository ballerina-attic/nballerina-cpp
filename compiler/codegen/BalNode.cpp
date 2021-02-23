#include "BalNode.h"

namespace nballerina {
BIRNode::BIRNode(Location *location) { setLocation(location); }

void BIRNode::translate(__attribute__((unused)) LLVMModuleRef &modRef) {}

BIRPackage *BIRNode::getPkgAddress() { return pkgAddress; }
void BIRNode::setPkgAddress(BIRPackage *pkgAddr) { pkgAddress = pkgAddr; }

} // namespace nballerina
