#ifndef __BALNODE__H__
#define __BALNODE__H__

#include "Debuggable.h"
#include "Translatable.h"

// forward declaration
class BIRPackage;

namespace nballerina {
class BIRNode : public Translatable, public Debuggable {
private:
  BIRPackage *pkg;

public:
  BIRNode() = default;
  BIRNode(Location *pos);
  BIRNode(BIRPackage *pkgptr);
  BIRNode(Location *pos, BIRPackage *pkgptr);
  virtual ~BIRNode() = default;
  BIRPackage *getPkgAddress();
  void setPkgAddress(BIRPackage *pkgptr);
  virtual void translate(LLVMModuleRef &modRef) override;
};

} // namespace nballerina

#endif //!__BALNODE__H__