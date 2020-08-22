#include "BIR.h"

Location::Location () 
{
}

Location::Location(string name, int line, int col)
{
  fileName    = name;
  lineNum     = line;
  columnNum   = col;
}

void Location::translate () 
{ 
}

Location::~Location ()
{
}
