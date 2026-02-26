#include "StringConverter.h"
#include <Windows.h>

std::wstring StringConvert::s2ws(const std::string& s, int cp)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(cp, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(cp, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string StringConvert::ws2s(const std::wstring& s, int cp)
{
	if (s.empty()) return std::string();
	int size_needed = WideCharToMultiByte(cp, 0, &s[0], (int)s.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(cp, 0, &s[0], (int)s.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

std::string StringConvert::cp_convert(const char* str, int from, int to)
{
	std::string res;
	int result_u, result_c;
	result_u = MultiByteToWideChar(from, 0, str, -1, 0, 0);
	if (!result_u) { return 0; }
	wchar_t* ures = new wchar_t[result_u];
	if (!MultiByteToWideChar(from, 0, str, -1, ures, result_u)) {
		delete[] ures;
		return 0;
	}
	result_c = WideCharToMultiByte(to, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c) {
		delete[] ures;
		return 0;
	}
	char* cres = new char[result_c];
	if (!WideCharToMultiByte(to, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		return 0;
	}
	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}

std::string StringConvert::cp_convert(std::string str, int from, int to)
{
	return cp_convert(str.c_str(), from, to);
}

std::string StringConvert::cp_from_sys_to_unicode(std::string str)
{
	return cp_convert(str, GetACP(), 65001);
}

std::string StringConvert::cp_from_unicode_to_sys(std::string str)
{
	return cp_convert(str, 65001, GetACP());
}
