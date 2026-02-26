#include "stdafx.h"
#include "IO.h"
#include "StringConverter.h"

#include "../boost/algorithm/string/replace.hpp"
#include "../boost/algorithm/string.hpp"
#include "../boost/filesystem.hpp"

#include <boost/iostreams/device/mapped_file.hpp>

using namespace std;

IO::IO()
{
}

IO::~IO()
{
}

bool IO::isDir(string dir)
{
	/*struct _stat fileInfo;
	_wstat(StringConvert::s2ws(dir, GetACP()).c_str(), &fileInfo);
	if (S_ISDIR(fileInfo.st_mode)) {
		return true;
	}
	else {
		return false;
	}*/

	wstring wdir = StringConvert::s2ws(dir, GetACP()).c_str();
	return boost::filesystem::is_directory(wdir);
}

bool IO::isDir(wstring dir)
{
	/*struct _stat fileInfo;
	_wstat(dir.c_str(), &fileInfo);
	if (S_ISDIR(fileInfo.st_mode)) {
		return true;
	}
	else {
		return false;
	}*/

	return boost::filesystem::is_directory(dir);
}

void IO::listFiles(string baseDir, bool recursive, std::function<void(string)> callback_d, std::function<void(string, string)> callback)
{
	_WDIR *dp;
	struct _wdirent *dirp;
	int cp = GetACP();
	const char* _dir = baseDir.c_str();// ::StringConvert::cp_convert(baseDir.c_str(), cp, 65001).c_str();
	std::wstring wstr = ::StringConvert::s2ws(_dir, cp);
	wstring _wbasedir = StringConvert::s2ws(baseDir, cp);

	if ((dp = _wopendir(wstr.c_str())) == NULL)
	{
		//cout << "[ERROR: " << errno << " ] Couldn't open " << baseDir << "." << endl;
		return;
	}
	else {
		while ((dirp = _wreaddir(dp)) != NULL) {
			if (dirp->d_name != wstring(L".") && dirp->d_name != wstring(L".."))
			{
				string _wname = ::StringConvert::ws2s(dirp->d_name, cp);
				string _str = baseDir + _wname;

				if (isDir(_wbasedir + dirp->d_name) == true && recursive == true)
				{
					if (callback_d != NULL)
						callback_d(_str);

					listFiles(_str + "/", true, callback_d, callback);
				}
				else
				{
					if (callback != NULL)
						callback(baseDir, _wname);
				}
			}
		}
		_wclosedir(dp);
	}
}

string IO::GetFileExtension(const std::string& FileName)
{
	if (FileName.length() > 0)
	{
		size_t i = FileName.find_last_of(".");

		if (i != std::string::npos)
		{
			if (FileName.length() > i)
			{
				string ext = FileName.substr(i + 1);
				ext = boost::algorithm::to_lower_copy(ext);
				
				return ext;
			}
		}
	}

	return "";
}

string IO::GetFileName(const std::string & FileName)
{
	if (FileName.find_last_of("/") != std::string::npos)
		return FileName.substr(FileName.find_last_of("/") + 1, FileName.find_last_of(".") - FileName.find_last_of("/") - 1);
	else if (FileName.find_last_of("\\") != std::string::npos)
		return FileName.substr(FileName.find_last_of("\\") + 1, FileName.find_last_of(".") - FileName.find_last_of("\\") - 1);
	else if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(0, FileName.find_last_of("."));
	else return FileName;

	return "";
}

string IO::GetFileNameWithExt(const std::string & FileName)
{
	if (FileName.find_last_of("/") != std::string::npos)
		return FileName.substr(FileName.find_last_of("/") + 1);
	else if (FileName.find_last_of("\\") != std::string::npos)
		return FileName.substr(FileName.find_last_of("\\") + 1);
	else
		return FileName;

	return "";
}

string IO::GetFilePath(const std::string & FileName)
{
	boost::filesystem::wpath p(FileName);
	boost::filesystem::wpath dir = p.remove_filename();
	std::string _fpath = dir.generic_string();

	if (!_fpath.empty())
		_fpath += "/";

	return _fpath;
}

string IO::RemovePart(const std::string& FileName, const std::string& part)
{
	std::string result = boost::replace_all_copy(FileName, part, "");

	return result;
}

bool IO::FileExists(const std::string & name)
{
	struct _stat buffer;
	return (_wstat(StringConvert::s2ws(name, GetACP()).c_str(), &buffer) == 0);
}

bool IO::DirExists(const std::string & name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

string IO::ReadText(string path)
{
	string result = "";

	std::ifstream in(path, std::ios::in);

	if (!in.is_open()) {
		return "";
	}

	std::string line;
	while (std::getline(in, line)) {
		result += line + "\n";
	}

	in.close();

	return result;
}

void IO::WriteText(string path, string text)
{
	ofstream myfile;
	myfile.open(path);
	myfile << text;
	myfile.close();
}

void IO::FileCopy(string from, string to)
{
	std::ifstream  src(from, std::ios::binary);
	std::ofstream  dst(to, std::ios::binary);

	dst << src.rdbuf();
}

void IO::DirCopy(string from, string to, bool recursive)
{
	if (recursive)
		std::filesystem::copy(std::filesystem::path(from), std::filesystem::path(to), std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
	else
		std::filesystem::copy(std::filesystem::path(from), std::filesystem::path(to), std::filesystem::copy_options::overwrite_existing);
}

boost::system::error_code IO::FileRename(string from, string to)
{
	boost::system::error_code err;
	boost::filesystem::rename(from, to, err);

	return err;

	//return rename(from.c_str(), to.c_str());
}

string IO::ReplaceBackSlashes(string input)
{
	std::string result = boost::replace_all_copy(input, "\\", "/");

	return result;
}

string IO::ReplaceFrontSlashes(string input)
{
	std::string result = boost::replace_all_copy(input, "/", "\\\\");

	return result;
}

void IO::CreateDir(string path, bool recursive)
{
	if (!recursive)
	{
		CreateDirectory(StringConvert::s2ws(path, GetACP()).c_str(), NULL);
	}
	else
	{
		boost::filesystem::create_directories(path);
	}
}

void IO::FileDelete(string path)
{
	std::remove(path.c_str());
}

std::vector<unsigned char> IO::ReadBinaryFile(const std::string filename)
{
	// binary mode is only for switching off newline translation
	std::ifstream file(filename, std::ios::binary);
	file.unsetf(std::ios::skipws);

	std::streampos file_size;
	file.seekg(0, std::ios::end);
	file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> vec(file_size);
	vec.insert(vec.begin(),
		std::istream_iterator<unsigned char>(file),
		std::istream_iterator<unsigned char>());
	return (vec);
}

void IO::DirDeleteRecursive(string path)
{
	try
	{
		if (boost::filesystem::exists(path))
		{
			boost::filesystem::remove_all(path);
		}
	}
	catch (boost::filesystem::filesystem_error const& e)
	{
		//display error message 
	}
}

string IO::RemoveLastLocation(string input)
{
	string str = input;

	if (str.length() > 0)
	{
		str = RemoveLastSlash(str);

		return GetFilePath(str);
	}

	return "";
}

std::string IO::RemoveLastSlash(std::string input)
{
	string str = input;

	if (str.length() > 0)
	{
		if (str[str.length() - 1] == '/')
			str = str.substr(0, str.length() - 2);

		if (str[str.length() - 1] == '\\')
			str = str.substr(0, str.length() - 2);

		return str;
	}

	return "";
}
