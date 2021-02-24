#include "BalTypeDecl.h"

namespace nballerina {

TypeDecl::TypeDecl(int tagp, std::string namep, int flagsp)
    : tag(tagp), name(std::move(namep)), flags(flagsp) {}

int TypeDecl::getTypeTag() { return tag; }
std::string TypeDecl::getTypeDeclName() { return name; }
int TypeDecl::getFlags() { return flags; }

MapTypeDecl::MapTypeDecl(int tag, std::string name, int flags, int memberTag)
    : TypeDecl{tag, name, flags}, memberTypeTag{memberTag} {}

int MapTypeDecl::getTypeMemberTag() { return memberTypeTag; }

} // namespace nballerina