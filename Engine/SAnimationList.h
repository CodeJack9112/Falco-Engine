#pragma once

#include "SComponent.h"
#include <string>

#include "../boost/serialization/vector.hpp"

struct SAnimationData
{
public:
	SAnimationData() {}
	~SAnimationData() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & name;
		ar & startFrame;
		ar & endFrame;
		ar & loop;
		ar & speed;
		ar & fileName;
		if (version > 1) ar& fileGuid;
	}

public:
	std::string name = "";
	std::string fileName = "";
	std::string fileGuid = "";
	int startFrame = 0;
	int endFrame = 100;
	bool loop = false;
	float speed = 1.0;
};

class SAnimationList : public SComponent
{
public:
	SAnimationList() {}
	~SAnimationList() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & BOOST_SERIALIZATION_NVP(animationData);

		ar & fbxFileName;
		ar & defaultAnimation;
		ar & playAtStart;
		if (version > 1) ar& fbxFileGuid;
	}

public:
	std::vector<SAnimationData> animationData;
	std::string fbxFileName = "";
	std::string fbxFileGuid = "";
	int defaultAnimation = 0;
	bool playAtStart = false;
};

BOOST_CLASS_VERSION(SAnimationData, 2)
BOOST_CLASS_VERSION(SAnimationList, 2)