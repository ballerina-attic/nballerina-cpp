#include "BIR.h"

Scope::Scope() {
}

Scope::Scope (Symbol *ownerp):owner(ownerp) {
}

Scope::Scope (Symbol *ownerp, map<string, ScopeEntry*> entriesp):
		owner(ownerp), entries(entriesp) {
}

ScopeEntry* Scope::getScopeEntry(string name) {
  map<string, ScopeEntry*>::iterator it;
  it = entries.find(name);

  if(it == entries.end()) {
    return NULL;
  }
  else
    return it->second;
}

void Scope::translate(LLVMModuleRef &modRef) {

}

Scope::~Scope() {
}

