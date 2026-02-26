#pragma once

#include "SComponent.h"
#include <string>

struct SWheelInfo
{
public:
	SWheelInfo() {}
	~SWheelInfo() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& m_connectedObjectName;
		ar& m_radius;
		ar& m_width;
		ar& m_suspensionStiffness;
		ar& m_suspensionDamping;
		ar& m_suspensionCompression;
		ar& m_suspensionRestLength;
		ar& m_friction;
		ar& m_rollInfluence;
		ar& m_direction;
		ar& m_axle;
		ar& m_connectionPoint;
		ar& m_isFrontWheel;
	}

public:
	std::string m_connectedObjectName = "[None]";
	float m_radius = 0.5f;
	float m_width = 0.4f;
	float m_suspensionStiffness = 20.0f;
	float m_suspensionDamping = 2.3f;
	float m_suspensionCompression = 4.4f;
	float m_suspensionRestLength = 0.6f;
	float m_friction = 1000.0f;
	float m_rollInfluence = 0.1f;
	SVector3 m_direction = SVector3(0, -1, 0);
	SVector3 m_axle = SVector3(-1, 0, 0);
	SVector3 m_connectionPoint = SVector3(0, 0, 0);
	bool m_isFrontWheel = false;
};

class SVehicle : public SComponent
{
public:
	SVehicle() {}
	~SVehicle() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::base_object<SComponent>(*this);
		ar& BOOST_SERIALIZATION_NVP(wheels);
		if (version > 1)
		{
			ar& vehicleAxis;
			ar& invertForward;
		}
	}

public:
	std::vector<SWheelInfo> wheels;

	SVector3 vehicleAxis = SVector3(0, 1, 2);
	bool invertForward = false;
};

BOOST_CLASS_VERSION(SVehicle, 2)
BOOST_CLASS_VERSION(SWheelInfo, 1)