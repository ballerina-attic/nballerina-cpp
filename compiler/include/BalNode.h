#ifndef __BALNODE__H__
#define __BALNODE__H__

#include "BalLocation.h"
#include "Translatable.h"

// forward declaration
class BIRPackage;

namespace nballerina {
class BIRNode : Translatable {
private:
  Location *loc;
  BIRPackage *pkgAddress;

public:
  BIRNode() = default;
  BIRNode(Location *pos);
  virtual ~BIRNode() = default;

  BIRPackage *getPkgAddress();
  Location *getLocation();
  void setLocation(Location *newLoc);
  void setPkgAddress(BIRPackage *pkgAddr);
  virtual void translate(LLVMModuleRef &modRef) override;
};

} // namespace nballerina

#endif //!__BALNODE__H__