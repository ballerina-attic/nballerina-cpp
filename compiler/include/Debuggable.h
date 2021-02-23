#ifndef __DEBUGGABLE__H__
#define __DEBUGGABLE__H__

#include "BalLocation.h"

namespace nballerina {
class Debuggable {
  Location *pos;

public:
  Debuggable() = default;
  virtual ~Debuggable() = default;
  Location *getLocation() { return pos; };
  void setLocation(Location *newPos) { pos = newPos; };
};

} // namespace nballerina

#endif //!__DEBUGGABLE__H__
