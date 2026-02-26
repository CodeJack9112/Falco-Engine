#pragma once

#include <string>
#include <iostream>
#include <atlstr.h>

#include "../Engine/StringConverter.h"
#include "../Engine/IO.h"

#include "../boost/filesystem/operations.hpp"
#include "../boost/filesystem/path.hpp"

using namespace std;
namespace fs = boost::filesystem;

class Helper
{
public:

	static string ExePath()
	{
		HMODULE hmod = GetModuleHandle(NULL);

		CString fullPath;
		DWORD pathLen = ::GetModuleFileName(hmod, fullPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH); // hmod of zero gets the main EXE
		fullPath.ReleaseBuffer(pathLen);
		fullPath.Replace(_T("\\"), _T("/"));
		string path = CSTR2STR(fullPath);
		path = IO::GetFilePath(path);

		return path;
	}

	static string ExeName()
	{
		HMODULE hmod = GetModuleHandle(NULL);

		CString fullPath;
		DWORD pathLen = ::GetModuleFileName(hmod, fullPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH); // hmod of zero gets the main EXE
		fullPath.ReleaseBuffer(pathLen);
		fullPath.Replace(_T("\\"), _T("/"));
		string path = CSTR2STR(fullPath);

		return path;
	}

	static string WorkingDir()
	{
		fs::path full_path(fs::current_path());

		return full_path.generic_string() + "/";
	}

	static std::string CSTR2STR(CString str)
	{
		std::string _str = ::StringConvert::ws2s(str.GetString(), GetACP());

		return _str;
	}
};