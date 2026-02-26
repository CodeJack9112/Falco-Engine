#include "PlayerPrefs.h"

#include "../boost/serialization/export.hpp"
#include "../Engine/IO.h"

using namespace std;

PlayerPrefs PlayerPrefs::Singleton;

void PlayerPrefs::load(std::string path)
{
	if (IO::FileExists(path))
	{
		//Deserialize
		std::ifstream ofs(path, std::ios::binary);
		boost::archive::binary_iarchive stream(ofs);

		//Read file
		stream >> *this;
		ofs.close();
		stream.delete_created_pointers();
	}
}

void PlayerPrefs::save(std::string path)
{
	//Prepare serialization
	std::ofstream ofs(path, std::ios::binary);
	boost::archive::binary_oarchive stream(ofs);

	//Write file
	stream << *this;
	ofs.close();
}

void PlayerPrefs::setInt(std::string key, int value)
{
	auto it = find_if(data.begin(), data.end(), [=](PlayerPrefsData& dt) -> bool
	{
		return dt.key == key;
	});

	if (it != data.end())
	{
		it->intVal = value;
	}
	else
	{
		PlayerPrefsData dt;
		dt.key = key;
		dt.intVal = value;
		data.push_back(dt);
	}
}

int PlayerPrefs::getInt(std::string key, int defaultValue)
{
	auto it = find_if(data.begin(), data.end(), [=](PlayerPrefsData& dt) -> bool
	{
		return dt.key == key;
	});

	if (it != data.end())
	{
		return it->intVal;
	}

	return defaultValue;
}

void PlayerPrefs::setFloat(std::string key, float value)
{
	auto it = find_if(data.begin(), data.end(), [=](PlayerPrefsData& dt) -> bool
	{
		return dt.key == key;
	});

	if (it != data.end())
	{
		it->floatVal = value;
	}
	else
	{
		PlayerPrefsData dt;
		dt.key = key;
		dt.floatVal = value;
		data.push_back(dt);
	}
}

float PlayerPrefs::getFloat(std::string key, float defaultValue)
{
	auto it = find_if(data.begin(), data.end(), [=](PlayerPrefsData& dt) -> bool
	{
		return dt.key == key;
	});

	if (it != data.end())
	{
		return it->floatVal;
	}

	return defaultValue;
}

void PlayerPrefs::setString(std::string key, std::string value)
{
	auto it = find_if(data.begin(), data.end(), [=](PlayerPrefsData& dt) -> bool
	{
		return dt.key == key;
	});

	if (it != data.end())
	{
		it->stringVal = value;
	}
	else
	{
		PlayerPrefsData dt;
		dt.key = key;
		dt.stringVal = value;
		data.push_back(dt);
	}
}

std::string PlayerPrefs::getString(std::string key, std::string defaultValue)
{
	auto it = find_if(data.begin(), data.end(), [=](PlayerPrefsData& dt) -> bool
	{
		return dt.key == key;
	});

	if (it != data.end())
	{
		return it->stringVal;
	}

	return defaultValue;
}

bool PlayerPrefs::hasKey(std::string key)
{
	auto it = find_if(data.begin(), data.end(), [=](PlayerPrefsData& dt) -> bool
	{
		return dt.key == key;
	});

	if (it != data.end())
	{
		return true;
	}

	return false;
}

void PlayerPrefs::deleteKey(std::string key)
{
	auto it = find_if(data.begin(), data.end(), [=](PlayerPrefsData& dt) -> bool
	{
		return dt.key == key;
	});

	if (it != data.end())
	{
		data.erase(it);
	}
}

void PlayerPrefs::deleteAll()
{
	data.clear();
}
