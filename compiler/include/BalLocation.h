#ifndef __BALLOCATIO__H__
#define __BALLOCATIO__H__

#include <string>

namespace nballerina {

class Location {
  private:
    std::string fileName;
    const int sLine;
    const int sCol;
    const int eLine;
    const int eCol;

  public:
    Location() = delete;
    Location(std::string name, int sline, int scol, int eline, int ecol);
    ~Location() = default;

    std::string getFileName();
    int getStartLineNum();
    int getStartColumnNum();
    int getEndLineNum();
    int getEndColumnNum();
};

} // namespace nballerina

#endif //!__BALLOCATIO__H__
