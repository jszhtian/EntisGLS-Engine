#pragma once
#include "Class_CsxSector.h"
#include "Class_CsxConstStr.h"
class Class_CsxImage :
	public Class_CsxSector
{
public:
	Class_CsxImage(sCsxSectorHeader sHeader, shared_ptr<char[]> c_SectorData) :Class_CsxSector(sHeader, c_SectorData) {};
	~Class_CsxImage();
	bool setConstStrSector(shared_ptr<Class_CsxConstStr> Sector);
	bool DumpStringInOrderToFile(string sFileName, unsigned int StartIndex = 0);
private:
	shared_ptr<Class_CsxConstStr> pSector;
};

