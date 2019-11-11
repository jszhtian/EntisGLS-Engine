#include "Class_CsxConstStr.h"

std::wstring FixStringW(std::wstring& Line)
{
	std::wstring Name;
	for (auto it : Line)
	{
		if (it == 0x000D)
		{
			Name += L"/r";
		}
		else if (it == 0x000A)
		{
			Name += L"/n";
		}
		else
		{
			Name += it;
		}
	}
	return Name;
}

std::wstring ReplaceCR(const wstring& orignStr)
{
	size_t pos = 0;
	wstring tempStr = orignStr;
	wstring oldStr1 = L"/n";
	wstring oldStr2 = L"/r";
	wstring newStr1 = L"\n";
	wstring newStr2 = L"\r";
	wstring::size_type newStrLen = newStr1.length();
	wstring::size_type oldStrLen = oldStr1.length();
	while (true)
	{
		pos = tempStr.find(oldStr1, pos);
		if (pos == wstring::npos) break;

		tempStr.replace(pos, oldStrLen, newStr1);
		pos += newStrLen;

	}
	pos = 0;
	while (true)
	{
		pos = tempStr.find(oldStr2, pos);
		if (pos == wstring::npos) break;

		tempStr.replace(pos, oldStrLen, newStr2);
		pos += newStrLen;

	}

	return tempStr;
}

Class_CsxConstStr::Class_CsxConstStr(sCsxSectorHeader sHeader, shared_ptr<char[]> c_SectorData) :Class_CsxSector(sHeader, c_SectorData)
{
	auto pTmpPtr = Class_CsxSector::GetSectorData().get();
	EntryCount = *reinterpret_cast <uint32_t*>(pTmpPtr);
	uint64_t offset = 0;
	offset += sizeof(uint32_t);
	for (unsigned int i = 0; i < EntryCount; i++)
	{
		auto iTextLen = *reinterpret_cast <uint32_t*>(pTmpPtr + offset);
		offset += sizeof(uint32_t);
		wstring sTmpStr(reinterpret_cast <wchar_t*>(pTmpPtr + offset), iTextLen);
		offset += iTextLen * 2;
		vConstStrList.push_back(sTmpStr);
		auto iDwordCount = *reinterpret_cast <uint32_t*>(pTmpPtr + offset);
		offset += sizeof(uint32_t);
		vector<uint32_t> vTmpDwordList;
		for (unsigned int i = 0; i < iDwordCount; i++)
		{
			auto iDword = *reinterpret_cast <uint32_t*>(pTmpPtr + offset + sizeof(uint32_t) * i);
			vTmpDwordList.push_back(iDword);
		}
		this->vDwordList.push_back(vTmpDwordList);
		offset += sizeof(uint32_t) * iDwordCount;
	}
}

Class_CsxConstStr::~Class_CsxConstStr()
{
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
		auto F_str = FixStringW(var);
		fTmpHandle.write(reinterpret_cast<char*>(const_cast<wchar_t*>(F_str.c_str())), F_str.length() * 2);
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
		auto F_str = FixStringW(this->vConstStrList.at(index));
		return F_str;
	}
}

bool Class_CsxConstStr::SetStringByIndex(wstring sText,uint32_t index)
{
	this->vConstStrList.at(index) = sText;
	return true;
}

bool Class_CsxConstStr::RebuildConstStrSector()
{
	if (this->vConstStrList.size() != this->vDwordList.size()||EntryCount!= this->vConstStrList.size())
	{
		return false;
	}
	this->CleanSectorData();
	auto pMemBuf = shared_ptr<char[]>(new char[1024*1024*10]);
	auto pRawMemPtr = pMemBuf.get();
	uint64_t offset = 0;
	memcpy(pRawMemPtr+ offset, &this->EntryCount, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	for (unsigned int i = 0; i < this->EntryCount; i++)
	{
		uint32_t iTextLen = this->vConstStrList.at(i).length();
		memcpy(pRawMemPtr + offset,&iTextLen,sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(pRawMemPtr + offset, this->vConstStrList.at(i).c_str(), iTextLen * 2);
		offset += iTextLen * 2;
		uint32_t iArrayCount = this->vDwordList.at(i).size();
		memcpy(pRawMemPtr + offset, &iArrayCount, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		for (auto var : this->vDwordList.at(i))
		{
			memcpy(pRawMemPtr + offset, &var, sizeof(uint32_t));
			offset += sizeof(uint32_t);
		}
	}
	this->SetSectorData(pMemBuf);
	this->sHeader.SectorSize = offset;
	return true;
}

bool Class_CsxConstStr::LoadConstStrFromFile(wstring sFileName)
{
	wifstream fTmpHandle;
	fTmpHandle.open(sFileName,ios::binary);
	setlocale(LC_ALL, "zh_CN.utf8");
	if (!fTmpHandle.is_open())
	{
		return false;
	}
	//Warning C4996 codecvt_utf16 deprecated in C++17
	//define Unicode convert
	fTmpHandle.imbue(
		std::locale(
			fTmpHandle.getloc(),
			new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>
		)
	);
	fTmpHandle.seekg(2, ios::beg);
	uint32_t index = 0;
	vector<wstring> getList;
	wchar_t TextBuf[0x100];
	while (true)
	{
		fTmpHandle.getline(TextBuf,0x100);
		wstring sText(TextBuf);
		//erase "\r" at end of String
		if (!sText.empty() && *sText.rbegin() == '\r') {
			sText.erase(sText.length() - 1, 1);
		}
		if (fTmpHandle.eof()) break;
		auto ReplaceText = ReplaceCR(sText);
		getList.push_back(ReplaceText);
	}
 	fTmpHandle.close();
	if (getList.size() != this->vConstStrList.size())
	{
		return false;
	}
	for (uint32_t i = 0; i < getList.size(); i++)
	{
		this->SetStringByIndex(getList.at(i),i);
	}
	return true;
}
