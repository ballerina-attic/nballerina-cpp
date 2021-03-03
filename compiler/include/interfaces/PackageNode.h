#ifndef __PACKAGENODE__H__
#define __PACKAGENODE__H__

namespace nballerina {

// Forward Declaration
class Package;

class PackageNode {
  public:
    virtual Package *getPackage() = 0;
};

} // namespace nballerina

#endif //!__PACKAGENODE__H__
