#pragma once

#include "SComponent.h"
#include <string>

class SRigidBody : public SComponent
{
public:
	SRigidBody() {}
	~SRigidBody() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & mass;
		ar & freezePositionX;
		ar & freezePositionY;
		ar & freezePositionZ;
		ar & freezeRotationX;
		ar & freezeRotationY;
		ar & freezeRotationZ;
		ar & isKinematic;
		if (version > 1) ar & isStatic;
		if (version > 2)
		{
			ar& useOwnGravity;
			ar& gravity;
			ar& friction;
			ar& bounciness;
			ar& linearDamping;
			ar& angularDamping;
		}
	}

public:
	float mass = 0;
	bool freezePositionX = false;
	bool freezePositionY = false;
	bool freezePositionZ = false;

	bool freezeRotationX = false;
	bool freezeRotationY = false;
	bool freezeRotationZ = false;

	bool isKinematic = false;
	bool isStatic = false;

	bool useOwnGravity = false;
	SVector3 gravity = SVector3(0, -9.81f, 0);
	float friction = 1.0f;
	float bounciness = 0.0f;
	float linearDamping = 0.0f;
	float angularDamping = 0.0f;
};

BOOST_CLASS_VERSION(SRigidBody, 3)