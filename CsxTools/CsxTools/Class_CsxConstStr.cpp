#include "Class_CsxConstStr.h"

std::wstring FixStringW(std::wstring& Line)
{
	std::wstring Name;
	for (auto it : Line)
	{
		if (it == 0x000D)
		{
			Name += L"\\r";
		}
		if (it == 0x000A)
		{
			Name += L"\\n";
		}
		else
		{
			Name += it;
		}
	}
	return Name;
}

Class_CsxConstStr::Class_CsxConstStr(sCsxSectorHeader sHeader, shared_ptr<char[]> c_SectorData) :Class_CsxSector(sHeader, c_SectorData)
{
	auto pTmpPtr = Class_CsxSector::GetSectorData().get();
	auto EntryCount = *reinterpret_cast <uint32_t*>(pTmpPtr);
	uint64_t offset = 0;
	offset += sizeof(uint32_t);
	for (int i = 0; i < EntryCount; i++)
	{
		auto iTextLen = *reinterpret_cast <uint32_t*>(pTmpPtr + offset);
		offset += sizeof(uint32_t);
		wstring sTmpStr(reinterpret_cast <wchar_t*>(pTmpPtr + offset), iTextLen);
		offset += iTextLen * 2;
		auto F_outStr = FixStringW(sTmpStr);
		vConstStrList.push_back(F_outStr);
		auto iDwordCount = *reinterpret_cast <uint32_t*>(pTmpPtr + offset);
		offset += sizeof(uint32_t);
		offset += sizeof(uint32_t) * iDwordCount;
	}
}

bool Class_CsxConstStr::DumpConstStrToFile(string sFileName)
{
	fstream fTmpHandle;
	fTmpHandle.open(sFileName, fstream::binary | fstream::out);
	if (!fTmpHandle.is_open())
	{
		return false;
	}
	fTmpHandle.write(UTF16LEBOM, 2);
	for (auto& var : vConstStrList)
	{
		fTmpHandle.write(reinterpret_cast<char*>(const_cast<wchar_t*>(var.c_str())), var.length() * 2);
		fTmpHandle.write(UTF16CR, 4);
	}
	fTmpHandle.close();
	return true;
}

wstring Class_CsxConstStr::GetStringByIndex(uint32_t index)
{
	if (index > this->vConstStrList.size())
	{
		return wstring(L"ERROR! Out of Index!");
	}
	else
	{
		return this->vConstStrList.at(index);
	}
}
