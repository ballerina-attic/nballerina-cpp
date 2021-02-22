#ifndef __BALTYPEDECL__H__
#define __BALTYPEDECL__H__

#include <llvm-c/Types.h>
#include <string>
#include <vector>

class TypeDecl {
private:
  int tag;
  std::string name;
  int flags;

public:
  TypeDecl() = default;
  TypeDecl(int tag, int flags);
  TypeDecl(int tag, std::string name, int flags);
  virtual ~TypeDecl() = default;

  void setTypeDeclName(std::string namep) { name = namep; }
  void setTypeTag(int tagp) { tag = tagp; }
  void setFlags(int flag) { flags = flag; }

  int getTypeTag() { return tag; }
  std::string getTypeDeclName() { return name; }
  int getFlags() { return flags; }

  virtual void translate(LLVMModuleRef &modRef);
};

// Extend TypeDecl for MapTypeDecl; to store member type info
class MapTypeDecl : public TypeDecl {
private:
  const int memberTypeTag;

public:
  MapTypeDecl() = delete;
  MapTypeDecl(int tag, std::string name, int flags, int memberTag)
      : TypeDecl{tag, name, flags}, memberTypeTag{memberTag} {};
  ~MapTypeDecl() = default;
  int getTypeMemberTag() { return memberTypeTag; }
};

class InvokableType : public TypeDecl {
private:
  std::vector<TypeDecl *> paramTypes;
  TypeDecl *restType;
  TypeDecl *returnType;

public:
  InvokableType() = default;
  InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *restTy,
                TypeDecl *retTy);
  InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *retTy);
  ~InvokableType() = default;

  TypeDecl *getReturnType() { return returnType; }
  TypeDecl *getRestType() { return restType; }
  TypeDecl *getParamType(int i) { return paramTypes[i]; }
  size_t getParamTypeCount() { return paramTypes.size(); }
  void setReturnType(TypeDecl *ty) { returnType = ty; }
  void setRestType(TypeDecl *ty) { restType = ty; }
  void addParamType(TypeDecl *ty) { paramTypes.push_back(ty); }
};

#endif //!__BALTYPEDECL__H__