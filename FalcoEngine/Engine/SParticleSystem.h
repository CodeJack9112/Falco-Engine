#pragma once

#include <vector>

#include "SSceneNode.h"
#include "SVector.h"
#include "SColor.h"

#include "../boost/serialization/vector.hpp"

class SParticleSystemParameter
{
public:
	SParticleSystemParameter() = default;
	SParticleSystemParameter(std::string _name, std::string _value) { name = _name; value = _value; }
	~SParticleSystemParameter() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & name;
		ar & value;
	}

	std::string name;
	std::string value;
};

class SParticleEmitter
{
public:
	SParticleEmitter() = default;
	~SParticleEmitter() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & enabled;
		ar & angle;
		ar & startColor;
		ar & direction;
		ar & emissionRate;
		ar & minDuration;
		ar & maxDuration;
		ar & minVelocity;
		ar & maxVelocity;
		ar & minRepeatDelay;
		ar & maxRepeatDelay;
		ar & minLifeTime;
		ar & maxLifeTime;
		ar & type;
		ar & BOOST_SERIALIZATION_NVP(parameters);
	}

	bool enabled = true;
	float angle = 0;
	SColor startColor;
	SVector3 direction;
	float emissionRate = 0;
	float minDuration = 0;
	float maxDuration = 0;
	float minVelocity = 0;
	float maxVelocity = 0;
	float minRepeatDelay = 0;
	float maxRepeatDelay = 0;
	float minLifeTime = 5;
	float maxLifeTime = 5;
	std::string type;
	std::vector<SParticleSystemParameter> parameters;
};

class SParticleAffector
{
public:
	SParticleAffector() = default;
	~SParticleAffector() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & type;
		ar & BOOST_SERIALIZATION_NVP(parameters);
	}

	std::string type;
	std::vector<SParticleSystemParameter> parameters;
};

class SParticleSystem : public SSceneNode
{
public:
	SParticleSystem() = default;
	~SParticleSystem() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SSceneNode>(*this);
		ar & emit;
		ar & material;
		ar & keepLocal;
		ar & castShadows;
		ar & speedFactor;
		ar & size;
		ar & BOOST_SERIALIZATION_NVP(emitters);
		ar & BOOST_SERIALIZATION_NVP(affectors);

		if (version > 1) ar& billboardType;
		if (version > 2) ar& materialGuid;
	}

	bool emit = true;
	std::string material;
	std::string materialGuid;
	bool keepLocal = false;
	bool castShadows = true;
	float speedFactor = 1.0;
	SVector2 size;
	std::string billboardType = "point";
	std::vector<SParticleEmitter> emitters;
	std::vector<SParticleAffector> affectors;
};

BOOST_CLASS_VERSION(SParticleSystemParameter, 1)
BOOST_CLASS_VERSION(SParticleEmitter, 1)
BOOST_CLASS_VERSION(SParticleAffector, 1)
BOOST_CLASS_VERSION(SParticleSystem, 3)