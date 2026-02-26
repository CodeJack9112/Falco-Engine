#pragma once

#include "SComponent.h"
#include <string>

class SNavMeshAgent : public SComponent
{
public:
	SNavMeshAgent() {}
	~SNavMeshAgent() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & targetObjectName;
		ar & radius;
		ar & height;
		ar & speed;
		ar & acceleration;
		if (version > 1) ar & rotationSpeed;
	}

public:
	std::string targetObjectName = "";
	float radius = 0.6;
	float height = 2.0;
	float speed = 3.5;
	float acceleration = 8.0;
	float rotationSpeed = 1.0;
};

BOOST_CLASS_VERSION(SNavMeshAgent, 2)