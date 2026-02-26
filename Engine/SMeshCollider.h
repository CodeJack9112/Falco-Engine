#pragma once

#include "SComponent.h"
#include <string>

class SMeshCollider : public SComponent
{
public:
	SMeshCollider() {}
	~SMeshCollider() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & convex;
		if (version > 1) ar& isTrigger;
	}

public:
	bool convex = false;
	bool isTrigger = false;
};

BOOST_CLASS_VERSION(SMeshCollider, 2)