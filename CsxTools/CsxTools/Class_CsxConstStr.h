#pragma once
#include "Class_CsxSector.h"
#include<fstream>
#include <vector>
#include <string>
const char UTF16LEBOM[] = { 0xFF,0xFE };
const char UTF16CR[] = { 0x0D,0x00,0x0A,0x00 };

class Class_CsxConstStr :
	public Class_CsxSector
{
public:
	Class_CsxConstStr(sCsxSectorHeader sHeader, shared_ptr<char[]> c_SectorData);
	bool DumpConstStrToFile(string sFileName);
	wstring GetStringByIndex(uint32_t index);
private:
	vector<wstring> vConstStrList;
};

