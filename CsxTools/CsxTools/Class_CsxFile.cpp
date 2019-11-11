#include "Class_CsxFile.h"
#include <iostream>



Class_CsxFile::Class_CsxFile(string sFileName)
{
	LoadCsxFile(sFileName);
}

Class_CsxFile::~Class_CsxFile()
{
}

bool Class_CsxFile::DumpSectorsToFile()
{
	for (auto& Sector : this->vCsxSectors)
	{
		fstream fTmpHandle;
		string sFileName(Sector->sHeader.Magic,8);
		fTmpHandle.open(sFileName, fstream::binary | fstream::out);
		if (!fTmpHandle.is_open())
		{
			return false;
		}
		fTmpHandle.write(Sector->GetSectorData().get(), Sector->sHeader.SectorSize);
		fTmpHandle.close();
	}
	return true;
}

bool Class_CsxFile::RebuildCsxToFile(string sFileName)
{
	fstream fTmpHandle;
	fTmpHandle.open(sFileName, fstream::binary | fstream::out);
	if (!fTmpHandle.is_open())
	{
		return false;
	}
	uint64_t u64_Transfer_Count = 0;
	fTmpHandle.write(reinterpret_cast <char*>(&this->sHeader), sizeof(sCsxHeader));
	for (auto& Sector : this->vCsxSectors)
	{
		fTmpHandle.write(reinterpret_cast <char*>(&Sector->sHeader), sizeof(Class_CsxSector::sCsxSectorHeader));
		u64_Transfer_Count += sizeof(Class_CsxSector::sCsxSectorHeader);
		fTmpHandle.write(Sector->GetSectorData().get(), Sector->sHeader.SectorSize);
		u64_Transfer_Count += Sector->sHeader.SectorSize;
	}
	fTmpHandle.seekp(0, fstream::beg);
	this->sHeader.CsxFileSize = u64_Transfer_Count;
	fTmpHandle.write(reinterpret_cast <char*>(&this->sHeader), sizeof(sCsxHeader));
	fTmpHandle.close();
	return true;
}

bool Class_CsxFile::DumpConstStrToFile(string sFileName)
{
	for (auto& Sector : this->vCsxSectors)
	{
		if (Sector->IsSectorMagicEqual(SectorConstStrMagic))
		{
			auto _sector = static_pointer_cast<Class_CsxConstStr>(Sector);
			_sector->DumpConstStrToFile(sFileName);
		}
	}
	return true;
}

bool Class_CsxFile::ReplaceSector(shared_ptr<Class_CsxSector> cSector)
{
	for (auto& Sector : this->vCsxSectors)
	{
		if (Sector->IsSectorMagicEqual(cSector))
		{
			Sector = move(cSector);
		}
	}
	return true;
}

bool Class_CsxFile::DumpStringInOrderToFile(string sFileName, unsigned int StartIndex)
{
	shared_ptr<Class_CsxConstStr> _ConstStr_sector;
	shared_ptr<Class_CsxImage> _Image_sector;
	for (auto& Sector : this->vCsxSectors)
	{
		if (Sector->IsSectorMagicEqual(SectorConstStrMagic))
		{
			_ConstStr_sector = static_pointer_cast<Class_CsxConstStr>(Sector);
		}
		if (Sector->IsSectorMagicEqual(SectorImageMagic))
		{
			_Image_sector = static_pointer_cast<Class_CsxImage>(Sector);
		}
	}
	_Image_sector->setConstStrSector(_ConstStr_sector);
	_Image_sector->DumpStringInOrderToFile(sFileName, StartIndex);
	return true;
}



void Class_CsxFile::LoadCsxFile(string sFileName)
{
	fHandle.open(sFileName, std::fstream::binary | std::fstream::in);
	if (!fHandle.is_open())
	{
		return;
	}
	fHandle.read(reinterpret_cast<char*>(&sHeader), sizeof(Class_CsxFile::sCsxHeader));
	while (true)
	{
		Class_CsxSector::sCsxSectorHeader sSectorHdr;
		fHandle.read(reinterpret_cast<char*>(&sSectorHdr), sizeof(Class_CsxSector::sCsxSectorHeader));
		if (fHandle.eof()) break;
		auto p_Sector_Data = shared_ptr<char[]>(new char[sSectorHdr.SectorSize]);
		fHandle.read(p_Sector_Data.get(), sSectorHdr.SectorSize);
		shared_ptr<Class_CsxSector> cSector;
		if (memcmp(reinterpret_cast<char*>(&sSectorHdr.Magic), SectorImageMagic, 8) == 0)
		{
			cSector = shared_ptr<Class_CsxSector>(new Class_CsxImage(sSectorHdr, p_Sector_Data));
		}
		else if (memcmp(reinterpret_cast<char*>(&sSectorHdr.Magic), SectorConstStrMagic, 8) == 0)
		{
			cSector= shared_ptr<Class_CsxSector>(new Class_CsxConstStr(sSectorHdr, p_Sector_Data));
		}
		else
		{
			cSector = shared_ptr<Class_CsxSector>(new Class_CsxSector(sSectorHdr, p_Sector_Data));
		}
			
		this->vCsxSectors.push_back(cSector);
	}
}

bool Class_CsxFile::IsFileOpened()
{
	return (this->fHandle) ? true:false;
}

bool Class_CsxFile::dummyTest()
{
	for (auto& Sector : this->vCsxSectors)
	{
		if (Sector->IsSectorMagicEqual(SectorConstStrMagic))
		{
			auto _sector = static_pointer_cast<Class_CsxConstStr>(Sector);
			_sector->LoadConstStrFromFile(L"Rep.txt");
			_sector->RebuildConstStrSector();
		}
	}
	return true;
}
