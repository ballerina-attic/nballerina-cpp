#ifndef __BALPARAM__H__
#define __BALPARAM__H__

#include <string>

namespace nballerina {

class RestParam {
private:
  std::string name;

public:
  RestParam() = default;
  ~RestParam() = default;

  std::string getName();
};

} // namespace nballerina

#endif //!__BALPARAM__H__
