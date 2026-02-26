#pragma once

#include "SComponent.h"
#include <string>

class SSphereCollider : public SComponent
{
public:
	SSphereCollider() {}
	~SSphereCollider() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & radius;
		ar & offset;
		if (version > 1) ar& isTrigger;
	}

public:
	float radius = 1;
	SVector3 offset = SVector3(0, 0, 0);
	bool isTrigger = false;
};

BOOST_CLASS_VERSION(SSphereCollider, 2)
