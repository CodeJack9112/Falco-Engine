#pragma once

#include "SComponent.h"
#include <string>

class SFixedJoint : public SComponent
{
public:
	SFixedJoint() {}
	~SFixedJoint() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::base_object<SComponent>(*this);
		ar& connectedObjectName;
		ar& anchor;
		ar& connectedAnchor;
		ar& linkedBodiesCollision;
	}

public:
	std::string connectedObjectName = "[None]";
	SVector3 anchor = SVector3(0, 0, 0);
	SVector3 connectedAnchor = SVector3(0, 0, 0);
	bool linkedBodiesCollision = true;
};

BOOST_CLASS_VERSION(SFixedJoint, 1)