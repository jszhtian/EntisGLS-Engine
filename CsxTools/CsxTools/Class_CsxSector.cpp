#include "Class_CsxSector.h"


Class_CsxSector::Class_CsxSector(sCsxSectorHeader sHeader,shared_ptr<char[]> c_SectorData)
{
	memcpy(this->sHeader.Magic, sHeader.Magic, sizeof(sCsxSectorHeader));
	this->sHeader.SectorSize = sHeader.SectorSize;
	SetSectorData(c_SectorData);
}

shared_ptr<char[]> Class_CsxSector::GetSectorData()
{
	return this->c_SectorData;
}

bool  Class_CsxSector::SetSectorData(shared_ptr<char[]> c_SectorData)
{
	this->c_SectorData = move(c_SectorData);
	return true;
}

bool Class_CsxSector::IsSectorMagicEqual(shared_ptr<Class_CsxSector> Sector)
{
	return (memcmp(this->sHeader.Magic,Sector->sHeader.Magic,sizeof(sCsxSectorHeader))==0) ? true:false;
}

bool Class_CsxSector::IsSectorMagicEqual(const char* Magic)
{
	return (memcmp(this->sHeader.Magic, Magic, 8) == 0) ? true : false;
}

