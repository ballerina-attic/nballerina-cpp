#ifndef __PACKAGENODE__H__
#define __PACKAGENODE__H__

// forward declaration
class BIRPackage;

namespace nballerina {
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
