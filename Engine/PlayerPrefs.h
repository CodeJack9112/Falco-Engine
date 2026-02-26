#pragma once

#include <string>
#include <vector>

//Boost headers
#include "../boost/serialization/serialization.hpp"
#include "../boost/serialization/binary_object.hpp"
#include "../boost/archive/binary_oarchive.hpp"
#include "../boost/archive/binary_iarchive.hpp"
#include "../boost/serialization/vector.hpp"

class PlayerPrefsData
{
public:
	PlayerPrefsData() = default;
	~PlayerPrefsData() = default;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& key;
		ar& stringVal;
		ar& intVal;
		ar& floatVal;
	}

	std::string key = "";
	
	std::string stringVal = "";
	int intVal = 0;
	float floatVal = 0;
};

class PlayerPrefs
{
private:
	std::vector<PlayerPrefsData> data;

public:
	PlayerPrefs() = default;
	~PlayerPrefs() = default;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(data);
	}

	void load(std::string path);
	void save(std::string path);

	void setInt(std::string key, int value);
	int getInt(std::string key, int defaultValue = 0);

	void setFloat(std::string key, float value);
	float getFloat(std::string key, float defaultValue = 0);

	void setString(std::string key, std::string value);
	std::string getString(std::string key, std::string defaultValue = "");

	bool hasKey(std::string key);
	void deleteKey(std::string key);
	void deleteAll();

	static PlayerPrefs Singleton;
};

BOOST_CLASS_VERSION(PlayerPrefs, 1)
BOOST_CLASS_VERSION(PlayerPrefsData, 1)