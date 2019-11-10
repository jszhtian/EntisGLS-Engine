#pragma once
#include "Class_CsxSector.h"
#include "Class_CsxImage.h"
#include "Class_CsxConstStr.h"
#include <fstream>
#include <vector>
using namespace std;
const char SectorImageMagic[] = "image   ";
const char SectorConstStrMagic[] = "conststr";

class Class_CsxFile
{
public:
#pragma pack(1)
	struct sCsxHeader
	{
		char Magic[8];
		uint32_t dwFileID;
		uint32_t dwReserved;
		char cFormatDesc[0x28];
		uint64_t CsxFileSize;
	};
#pragma pack()
	Class_CsxFile(string sFileName);
	bool DumpSectorsToFile();
	bool RebuildCsxToFile(string sFileName);
	bool DumpConstStrToFile(string sFileName);
	bool ReplaceSector(shared_ptr<Class_CsxSector> cSector);
	bool DumpStringInOrderToFile(string sFileName,int StartIndex=0);
	void LoadCsxFile(string sFileName);
	bool IsFileOpened();
private:
	fstream fHandle;
	sCsxHeader sHeader;
	vector<shared_ptr<Class_CsxSector>> vCsxSectors;
};

