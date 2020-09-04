#include "BIR.h"

ScopeEntry::ScopeEntry () {
}

ScopeEntry::ScopeEntry (Symbol *sym, ScopeEntry *nextp):
		symbol(sym), next(nextp) {
}

ScopeEntry::~ScopeEntry() {

}
