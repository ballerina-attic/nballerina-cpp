//#include "BIRReader.h"
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

	struct BinBIRPackage readBIRPackage;
	char buf;
	std::string org;
        std::string name;
        std::string value;

	dataInStream.read((char*)&buf, sizeof(buf));
	for (int i = 0; i < (int)buf; i++)
		org += buf;
	dataInStream.read((char*)&buf, sizeof(buf));
	for (int i = 0; i < (int)buf; i++)
		name += buf;
	dataInStream.read((char*)&buf, sizeof(buf));
	for (int i = 0; i < (int)buf; i++)
		value += buf;

	readBIRPackage.org = org;
	readBIRPackage.name = name;
	readBIRPackage.value = value;

	//Reader.deserialize(Reader->dataInStream);
}
