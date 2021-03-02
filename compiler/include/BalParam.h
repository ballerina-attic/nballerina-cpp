#ifndef __BALPARAM__H__
#define __BALPARAM__H__

#include "interfaces/Debuggable.h"
#include "interfaces/PackageNode.h"
#include <string>

namespace nballerina {

// Forward Declaration
class Type;

class Param : public PackageNode, public Debuggable {
private:
  std::string name;
  int flags;
  Type *type;

public:
  Param() = default;
  Param(Location *pos, std::string name, int flags, Type *ty);
  ~Param() = default;

  int getFlags();
  std::string getName();
  Type *getTypeDecl();

  void setFlags(int newFlags);
  void setName(std::string newName);
  void setTypeDecl(Type *newType);
};

} // namespace nballerina

#endif //!__BALPARAM__H__