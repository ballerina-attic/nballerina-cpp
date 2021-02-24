#ifndef __PACKAGENODE__H__
#define __PACKAGENODE__H__

namespace nballerina {

// Forward Declaration
class BIRPackage;

class PackageNode {
private:
  BIRPackage *pkg;

public:
  PackageNode() = default;
  virtual ~PackageNode() = default;
  BIRPackage *getPkgAddress() { return pkg; };
  void setPkgAddress(BIRPackage *pkgPtr) { pkg = pkgPtr; };
};

} // namespace nballerina

#endif //!__PACKAGENODE__H__
