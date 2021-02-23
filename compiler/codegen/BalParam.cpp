#include "BalParam.h"

namespace nballerina {
Param::Param(Location *pos, std::string namep, int flagsp, TypeDecl *typep)
    : name(namep), flags(flagsp), type(typep) {
  setLocation(pos);
}

int Param::getFlags() { return flags; }
std::string Param::getName() { return name; }
TypeDecl *Param::getTypeDecl() { return type; }

void Param::setFlags(int newFlags) { flags = newFlags; }
void Param::setName(std::string newName) { name = newName; }
void Param::setTypeDecl(TypeDecl *newType) { type = newType; }

} // namespace nballerina
