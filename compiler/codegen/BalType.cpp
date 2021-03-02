#include "BalType.h"

namespace nballerina {

Type::Type(int tagp, std::string namep, int flagsp)
    : tag(tagp), name(std::move(namep)), flags(flagsp) {}

int Type::getTypeTag() { return tag; }
std::string Type::getTypeDeclName() { return name; }
int Type::getFlags() { return flags; }

MapTypeDecl::MapTypeDecl(int tag, std::string name, int flags, int memberTag)
    : Type{tag, name, flags}, memberTypeTag{memberTag} {}

int MapTypeDecl::getTypeMemberTag() { return memberTypeTag; }

} // namespace nballerina