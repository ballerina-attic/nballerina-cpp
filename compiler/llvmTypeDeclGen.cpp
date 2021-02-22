#include "BIR.h"
#include "BalTypeDecl.h"

TypeDecl::TypeDecl(int tagp, string namep, int flagsp)
    : tag(tagp), name(std::move(namep)), flags(flagsp) {}

int TypeDecl::getTypeTag() { return tag; }
std::string TypeDecl::getTypeDeclName() { return name; }
int TypeDecl::getFlags() { return flags; }

MapTypeDecl::MapTypeDecl(int tag, std::string name, int flags, int memberTag)
    : TypeDecl{tag, name, flags}, memberTypeTag{memberTag} {}

int MapTypeDecl::getTypeMemberTag() { return memberTypeTag; }
