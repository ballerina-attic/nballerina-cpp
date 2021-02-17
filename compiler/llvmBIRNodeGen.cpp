#include "BIR.h"

BIRNode::BIRNode(Location *location) : loc(location) {}

void BIRNode::translate(__attribute__((unused)) LLVMModuleRef &modRef) {}
