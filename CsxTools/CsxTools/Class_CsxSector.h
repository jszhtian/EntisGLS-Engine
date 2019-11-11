#pragma once
#include <memory>
#include <string>
#include <iostream>
using namespace std;


class Class_CsxSector
{
public:
#pragma pack(1)
	struct sCsxSectorHeader
	{
		char Magic[8];
		uint64_t SectorSize;
	};
#pragma pack()
	sCsxSectorHeader sHeader;
	~Class_CsxSector();
	Class_CsxSector(sCsxSectorHeader sHeader,shared_ptr<char[]> c_SectorData);
	shared_ptr<char[]> GetSectorData();
	bool SetSectorData(shared_ptr<char[]> c_SectorData);
	bool IsSectorMagicEqual(shared_ptr<Class_CsxSector> Sector);
	bool IsSectorMagicEqual(const char* Magic);
	bool CleanSectorData();

private:
	shared_ptr<char[]> c_SectorData;
};

