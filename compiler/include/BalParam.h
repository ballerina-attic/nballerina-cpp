#ifndef __BALPARAM__H__
#define __BALPARAM__H__

#include "interfaces/Debuggable.h"
#include "interfaces/PackageNode.h"
#include <string>

namespace nballerina {

// Forward Declaration
class TypeDecl;

class Param : public PackageNode, public Debuggable {
private:
  std::string name;
  int flags;
  TypeDecl *type;

public:
  Param() = default;
  Param(Location *pos, std::string name, int flags, TypeDecl *ty);
  ~Param() = default;

  int getFlags();
  std::string getName();
  TypeDecl *getTypeDecl();

  void setFlags(int newFlags);
  void setName(std::string newName);
  void setTypeDecl(TypeDecl *newType);
};

} // namespace nballerina

#endif //!__BALPARAM__H__