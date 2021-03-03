#include "BalType.h"

namespace nballerina {

Type::Type(TypeTag type, std::string namep, int flagsp) : type(type), name(std::move(namep)), flags(flagsp) {}

TypeTag Type::getTypeTag() { return type; }
std::string Type::getName() { return name; }
int Type::getFlags() { return flags; }

const char *Type::getNameOfType(TypeTag typeTag) {
    switch (typeTag) {
    case TYPE_TAG_INT:
        return "int";
    case TYPE_TAG_FLOAT:
        return "float";
    case TYPE_TAG_CHAR_STRING:
    case TYPE_TAG_STRING:
        return "string";
    case TYPE_TAG_BOOLEAN:
        return "bool";
    case TYPE_TAG_ANY:
        return "any";
    default:
        return "";
    }
}

MapTypeDecl::MapTypeDecl(TypeTag type, std::string name, int flags, TypeTag memType)
    : Type{type, name, flags}, memberType{memType} {}

TypeTag MapTypeDecl::getMemberTypeTag() { return memberType; }

} // namespace nballerina
