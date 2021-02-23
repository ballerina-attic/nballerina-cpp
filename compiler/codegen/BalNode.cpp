#include "BalNode.h"

namespace nballerina {
BIRNode::BIRNode(Location *location) : loc(location) {}

void BIRNode::translate(__attribute__((unused)) LLVMModuleRef &modRef) {}

BIRPackage *BIRNode::getPkgAddress() { return pkgAddress; }
Location *BIRNode::getLocation() { return loc; }
void BIRNode::setLocation(Location *newLoc) { loc = newLoc; }
void BIRNode::setPkgAddress(BIRPackage *pkgAddr) { pkgAddress = pkgAddr; }

} // namespace nballerina
