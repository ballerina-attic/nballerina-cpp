#ifndef __VARIABLEKIND__H__
#define __VARIABLEKIND__H__

#include <string>

namespace nballerina {

enum VarKind {
    LOCAL_VAR_KIND = 1,
    ARG_VAR_KIND = 2,
    TEMP_VAR_KIND = 3,
    RETURN_VAR_KIND = 4,
    GLOBAL_VAR_KIND = 5,
    SELF_VAR_KIND = 6,
    CONSTANT_VAR_KIND = 7
};

class VariableKind {
  private:
    std::string name;
    VarKind kind;

  public:
    VariableKind() = delete;
    VariableKind(std::string _name, VarKind _kind) : name(_name), kind(_kind) {}
    virtual ~VariableKind() = default;

    VarKind getKind() { return kind; }
    std::string &getName() { return name; };
};

} // namespace nballerina

#endif //!__VARIABLEKIND__H__
