#pragma once

#include "SComponent.h"
#include <string>

class SHingeJoint : public SComponent
{
public:
	SHingeJoint() {}
	~SHingeJoint() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & connectedObjectName;
		ar & anchor;
		ar & connectedAnchor;
		ar & axis;
		ar & limitMin;
		ar & limitMax;
		if (version > 1) ar& linkedBodiesCollision;
	}

public:
	std::string connectedObjectName = "[None]";
	SVector3 anchor = SVector3(0, 0, 0);
	SVector3 connectedAnchor = SVector3(0, 0, 0);
	SVector3 axis = SVector3(1, 0, 0);
	float limitMin = -3.14f;
	float limitMax = 3.14f;
	bool linkedBodiesCollision = true;
};

BOOST_CLASS_VERSION(SHingeJoint, 2)