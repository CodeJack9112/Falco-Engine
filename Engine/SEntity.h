#pragma once

#include "SSceneNode.h"
#include <string>

class SSubEntity
{
public:
	SSubEntity() = default;
	~SSubEntity() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & index;
		ar & materialName;
		if (version > 1) ar& materialGuid;
	}

	int index;
	string materialName;
	string materialGuid;
};

class SEntity : public SSceneNode
{
public:
	SEntity() = default;
	~SEntity() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SSceneNode>(*this);
		ar & mesh;
		ar & meshPath;
		ar & meshIndex;
		ar & isFbx;
		ar & castShadows;
		ar & receiveShadows;
		ar & BOOST_SERIALIZATION_NVP(subEntities);
		if (version > 1)
		{
			ar& useCustomLightmapSize;
			ar& customLightmapSize;
		}
		if (version > 2) ar& meshGuid;
	}

public:
	std::string mesh;
	std::string meshPath;
	std::string meshGuid;
	int meshIndex = -1;
	bool isFbx = false;
	bool useCustomLightmapSize = false;
	int customLightmapSize = 256;
	bool castShadows;
	bool receiveShadows;
	std::vector<SSubEntity> subEntities;
};

BOOST_CLASS_VERSION(SSubEntity, 2)
BOOST_CLASS_VERSION(SEntity, 3)