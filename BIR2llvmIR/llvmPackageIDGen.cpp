#include "BIR.h"

PackageID::PackageID() {}

PackageID::PackageID(string orgName, list<string> nameComps, string version) {}

PackageID::PackageID(string orgName, string name, string version) {}

PackageID::PackageID(string sourceFileName) {}

PackageID::~PackageID() {}

void PackageID::translate(LLVMModuleRef &modRef) { }
