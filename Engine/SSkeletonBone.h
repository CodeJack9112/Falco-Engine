#pragma once

#include "SComponent.h"
#include <string>

class SSkeletonBone : public SComponent
{
public:
	SSkeletonBone() {}
	~SSkeletonBone() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & index;
		ar & name;
		ar & bindingPosition;
		ar & position;
		ar & bindingRotation;
		ar & rotation;
		ar & skeletonName;
		ar & rootNodeName;

		if (version > 1)
		{
			ar& scale;
			ar& bindingScale;
		}

		if (version > 2) ar& isRootBone;
		if (version > 3) ar& skeletonGuid;
	}

public:
	int index = -1;
	std::string name;
	SVector3 bindingPosition;
	SVector3 position;
	SVector3 bindingScale;
	SVector3 scale;
	SQuaternion bindingRotation;
	SQuaternion rotation;
	std::string skeletonName = "";
	std::string skeletonGuid = "";
	std::string rootNodeName = "";
	bool isRootBone = false;
};

BOOST_CLASS_VERSION(SSkeletonBone, 4)