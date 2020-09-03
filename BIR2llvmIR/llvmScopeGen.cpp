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
    cout<<"ScopeEntry is Not Present in Map";
    return NULL;
  }
  else
    return it->second;
}

void Scope::translate(LLVMModuleRef &modRef) {

}

Scope::~Scope() {
}

