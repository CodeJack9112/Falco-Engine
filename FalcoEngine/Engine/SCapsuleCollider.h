#pragma once

#include "SComponent.h"
#include <string>

class SCapsuleCollider : public SComponent
{
public:
	SCapsuleCollider() {}
	~SCapsuleCollider() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & radius;
		ar & height;
		ar & offset;
		ar & rotation;
		if (version > 1) ar& isTrigger;
	}

public:
	float radius = 1.0;
	float height = 2.0;
	SVector3 offset = SVector3(0, 0, 0);
	SQuaternion rotation = SQuaternion(0, 0, 0, 1);
	bool isTrigger = false;
};

BOOST_CLASS_VERSION(SCapsuleCollider, 2)
