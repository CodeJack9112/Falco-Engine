#pragma once

#include "SComponent.h"
#include <string>

class SBoxCollider : public SComponent
{
public:
	SBoxCollider() {}
	~SBoxCollider() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & boxSize;
		ar & offset;
		ar & rotation;
		if (version > 1) ar& isTrigger;
	}

public:
	SVector3 boxSize = SVector3(1, 1, 1);
	SVector3 offset = SVector3(0, 0, 0);
	SQuaternion rotation = SQuaternion(0, 0, 0, 1);
	bool isTrigger = false;
};

BOOST_CLASS_VERSION(SBoxCollider, 2)
