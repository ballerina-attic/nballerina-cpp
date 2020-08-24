#include "BIR.h"
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
using namespace std;

int main()
{
	ifstream dataInStream;
	//BIRReader *Reader = new BIRReader();

        int size = 0;
        dataInStream.open("main-bir-dump", ios::in|ios::binary);
        dataInStream.seekg(0, ios::end);
        size = (int)dataInStream.tellg();
        dataInStream.seekg (0, ios::beg);

	//Reader.deserialize(Reader->dataInStream);
        BIRPackage pkg;
	pkg.translate();
}
