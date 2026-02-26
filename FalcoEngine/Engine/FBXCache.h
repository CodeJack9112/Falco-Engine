#pragma once

//Boost headers
#include "../boost/serialization/serialization.hpp"
#include "../boost/serialization/binary_object.hpp"
#include "../boost/archive/binary_oarchive.hpp"
#include "../boost/archive/binary_iarchive.hpp"
#include "../boost/serialization/vector.hpp"

#include <string>
#include "SVector.h"
#include "SQuaternion.h"

using namespace std;

class FBXNormalCache
{
public:
	FBXNormalCache() {}
	~FBXNormalCache() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(data);
	}

	std::vector<SVector3> data;
};

class FBXAnimationCache
{
public:
	FBXAnimationCache() {}
	~FBXAnimationCache() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & frame;
		ar & skeletal;
		ar & position;
		ar & rotation;
		ar & scale;
	}

	int frame = 0;
	bool skeletal = false;
	SVector3 position;
	SQuaternion rotation;
	SVector3 scale;
};

class FBXAnimationNode
{
public:
	FBXAnimationNode() {}
	~FBXAnimationNode() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & uid;
		ar & BOOST_SERIALIZATION_NVP(keyFrames);
	}

	int uid = -1;
	std::vector<FBXAnimationCache> keyFrames;
};

class FBXBlendWeightsCache
{
public:
	FBXBlendWeightsCache() {}
	~FBXBlendWeightsCache() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & weight;
		ar & jointname;
		ar & vertexid;
	}

	double weight = 0;
	std::string jointname = "";
	int vertexid = 0;
};

class FBXLightmapUVCache
{
public:
	FBXLightmapUVCache() {}
	~FBXLightmapUVCache() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(data);
	}

	std::vector<SVector2> data;
};

class FBXDataNode
{
public:
	FBXDataNode() {}
	~FBXDataNode() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & uid;
		ar & BOOST_SERIALIZATION_NVP(normalCacheData);
		ar & BOOST_SERIALIZATION_NVP(blendWeightsCacheData);
		ar & BOOST_SERIALIZATION_NVP(lightmapUVCache);
	}

	int uid = -1;
	std::vector<FBXNormalCache> normalCacheData;
	std::vector<FBXBlendWeightsCache> blendWeightsCacheData;
	std::vector<FBXLightmapUVCache> lightmapUVCache;
};

class FBXCache
{
public:
	FBXCache() {}
	~FBXCache() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & index;
		ar & BOOST_SERIALIZATION_NVP(dataCache);
		ar & BOOST_SERIALIZATION_NVP(animationCache);
	}

	int index = 0;
	std::vector<FBXDataNode> dataCache;
	std::vector<FBXAnimationNode> animationCache;

	void Load(string path);
	void Save(string path);
};

BOOST_CLASS_VERSION(FBXCache, 1)
BOOST_CLASS_VERSION(FBXNormalCache, 1)
BOOST_CLASS_VERSION(FBXAnimationCache, 1)
BOOST_CLASS_VERSION(FBXBlendWeightsCache, 1)
BOOST_CLASS_VERSION(FBXAnimationNode, 1)
BOOST_CLASS_VERSION(FBXDataNode, 1)
BOOST_CLASS_VERSION(FBXLightmapUVCache, 1)