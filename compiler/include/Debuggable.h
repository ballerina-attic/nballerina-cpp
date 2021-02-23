#ifndef __DEBUGGABLE__H__
#define __DEBUGGABLE__H__

#include "BalLocation.h"

namespace nballerina {
class Debuggable {
private:
  Location *loc;

public:
  Debuggable() = default;
  virtual ~Debuggable() = default;
  Location *getLocation() { return loc; };
  void setLocation(Location *newLoc) { loc = newLoc; };
};

} // namespace nballerina

#endif //!__DEBUGGABLE__H__