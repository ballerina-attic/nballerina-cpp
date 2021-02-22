#ifndef __BALTYPEDECL__H__
#define __BALTYPEDECL__H__

#include <llvm-c/Types.h>
#include <string>
#include <vector>

class TypeDecl {
private:
  const int tag;
  std::string name;
  const int flags;

public:
  TypeDecl() = delete;
  TypeDecl(int tag, std::string name, int flags);
  virtual ~TypeDecl() = default;

  int getTypeTag();
  std::string getTypeDeclName();
  int getFlags();
};

// Extend TypeDecl for MapTypeDecl; to store member type info
class MapTypeDecl : public TypeDecl {
private:
  const int memberTypeTag;

public:
  MapTypeDecl() = delete;
  MapTypeDecl(int tag, std::string name, int flags, int memberTag);
  ~MapTypeDecl() = default;
  int getTypeMemberTag();
};

class InvokableType {
private:
  std::vector<TypeDecl *> paramTypes;
  TypeDecl *restType;
  TypeDecl *returnType;

public:
  InvokableType() = delete;
  InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *restTy,
                TypeDecl *retTy);
  InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *retTy);
  ~InvokableType() = default;

  TypeDecl *getReturnType();
  TypeDecl *getRestType();
  TypeDecl *getParamType(int i);
  size_t getParamTypeCount();
};

#endif //!__BALTYPEDECL__H__