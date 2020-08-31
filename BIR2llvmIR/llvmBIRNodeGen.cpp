#include "BIR.h"

BIRNode::BIRNode () 
{
}

BIRNode::BIRNode (Location  location):loc(location)
{
}

void BIRNode::translate (LLVMModuleRef modRef) 
{
  cout << "Value of ary is: " << endl;  
}

BIRNode::~BIRNode ()
{
}
