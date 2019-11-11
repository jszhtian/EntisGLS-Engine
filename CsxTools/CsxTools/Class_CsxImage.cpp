#include "Class_CsxImage.h"

Class_CsxImage::~Class_CsxImage()
{
}

bool Class_CsxImage::setConstStrSector(shared_ptr<Class_CsxConstStr> Sector)
{
	this->pSector=Sector;
	return true;
}

bool Class_CsxImage::DumpStringInOrderToFile(string sFileName, unsigned int StartIndex)
{
	fstream fTmpHandle;
	fTmpHandle.open(sFileName, fstream::binary | fstream::out);
	if (!fTmpHandle.is_open())
	{
		return false;
	}
	fTmpHandle.write(UTF16LEBOM, 2);
	auto pImageDataPtr = Class_CsxSector::GetSectorData().get();
	unsigned char find_bytes[] = { 0x02,0x00,0x06,0x00,0x00,0x00,0x80 };
	for (uint32_t i = 0; i<this->sHeader.SectorSize - sizeof(find_bytes); i++)
	{
		auto cTmpPtr = &pImageDataPtr[i];
		if (memcmp(cTmpPtr, find_bytes, sizeof(find_bytes))==0)
		{
			auto index = *reinterpret_cast<uint32_t*>(cTmpPtr+ sizeof(find_bytes));
			if (index >= StartIndex)
			{
				auto stext = this->pSector->GetStringByIndex(index);
				fTmpHandle.write(reinterpret_cast<char*>(const_cast<wchar_t*>(stext.c_str())), stext.length() * 2);
				fTmpHandle.write(UTF16CR, 4);
			}
		}
	}
	fTmpHandle.close();
	return true;
}
