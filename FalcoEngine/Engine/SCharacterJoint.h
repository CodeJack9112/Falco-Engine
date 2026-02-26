#pragma once

#include "SComponent.h"
#include <string>

class SCharacterJoint : public SComponent
{
public:
	SCharacterJoint() {}
	~SCharacterJoint() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & connectedObjectName;
		ar & anchor;
		ar & connectedAnchor;
		ar & limitMin;
		ar & limitMax;
		if (version > 1) ar & axis;
		if (version > 2) ar& linkedBodiesCollision;
		if (version > 3) ar& linearAxis;
	}

public:
	std::string connectedObjectName = "[None]";
	SVector3 anchor = SVector3(0, 0, 0);
	SVector3 connectedAnchor = SVector3(0, 0, 0);
	SVector3 limitMin = SVector3(-3.14, -3.14, -3.14);
	SVector3 limitMax = SVector3(3.14, 3.14, 3.14);
	SVector3 axis = SVector3(1, 1, 1);
	SVector3 linearAxis = SVector3(1, 1, 1);
	bool linkedBodiesCollision = true;
};

BOOST_CLASS_VERSION(SCharacterJoint, 4)