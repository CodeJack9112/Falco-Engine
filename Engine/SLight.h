#pragma once

#include "SSceneNode.h"
#include "SColor.h"
#include "SVector.h"

class SLight : public SSceneNode
{
public:
	SLight() = default;
	~SLight() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SSceneNode>(*this);
		ar & range;
		ar & intensity;
		ar & innerAngle;
		ar & outerAngle;
		ar & falloff;
		ar & diffuseColor;
		ar & specularColor;
		ar & castShadows;
		ar & parameters;
		if (version > 1) ar& mode;
		if (version > 2) ar& shadowBias;
	}

public:
	float range;
	float intensity;
	float shadowBias = 0.0005f;
	float innerAngle;
	float outerAngle;
	float falloff;
	SColor diffuseColor;
	SColor specularColor;
	bool castShadows;
	SVector4 parameters;
	int mode = 0;
};

BOOST_CLASS_VERSION(SLight, 3)