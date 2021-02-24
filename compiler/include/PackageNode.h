#ifndef __PACKAGENODE__H__
#define __PACKAGENODE__H__

namespace nballerina {

// Forward Declaration
class Package;

class PackageNode {
private:
  Package *pkg;

public:
  PackageNode() = default;
  virtual ~PackageNode() = default;
  Package *getPkgAddress() { return pkg; };
  void setPkgAddress(Package *pkgPtr) { pkg = pkgPtr; };
};

} // namespace nballerina

#endif //!__PACKAGENODE__H__
