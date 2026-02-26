#pragma once

#include <vector>
#include <map>
#include "../boost/serialization/vector.hpp"
#include <OgreResource.h>

using namespace std;

class ResourceID
{
public:
	ResourceID() {}
	~ResourceID() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& resourceName;
		ar& resourceGuid;
	}

	std::string resourceName = "";
	std::string resourceGuid = "";
};

class ResourceMap
{
private:
	static std::vector<string> imgs;

public:
	ResourceMap() {}
	~ResourceMap() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(resourceMap);
	}

	static std::vector<ResourceID> resourceMap;
	static std::map<Ogre::Resource*, string> guidMap;

	static string getResourceNameFromGuid(string guid);
	static string getResourceGuidFromName(string name);
	static void setResourceGuid(string name, string guid);
	static void setResourceNameByGuid(string guid, string name);
	static void setResourceName(string oldName, string newName);

	static void load();
	static void save();

	static std::string genGuid();
	static void identifyResources();
	static void removeLostResources();
	static void addResource(string name, string guid);
};

class ResourceMapTemp
{
public:
	ResourceMapTemp() {}
	~ResourceMapTemp() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(resourceMap);
	}

	std::vector<ResourceID> resourceMap;

	void load(std::string fileName);
	void save(std::string fileName);
};

BOOST_CLASS_VERSION(ResourceID, 1)
BOOST_CLASS_VERSION(ResourceMap, 1)
BOOST_CLASS_VERSION(ResourceMapTemp, 1)