#ifndef __BALTYPEDECL__H__
#define __BALTYPEDECL__H__

#include <string>

namespace nballerina {

class Type {
private:
  const int tag;
  std::string name;
  const int flags;

public:
  Type() = delete;
  Type(int tag, std::string name, int flags);
  virtual ~Type() = default;

  int getTypeTag();
  std::string getTypeDeclName();
  int getFlags();
};

// Extend Type for MapTypeDecl; to store member type info
class MapTypeDecl : public Type {
private:
  const int memberTypeTag;

public:
  MapTypeDecl() = delete;
  MapTypeDecl(int tag, std::string name, int flags, int memberTag);
  ~MapTypeDecl() = default;
  int getTypeMemberTag();
};

} // namespace nballerina

#endif //!__BALTYPEDECL__H__