#pragma once

#include "SComponent.h"
#include <string>

class SAudioSource : public SComponent
{
public:
	SAudioSource() {}
	~SAudioSource() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		ar & fileName;
		ar & playOnStart;
		ar & loop;
		if (version > 1) ar& fileGuid;
		if (version > 2) ar& volume;
		if (version > 3)
		{
			ar& minDistance;
			ar& maxDistance;
		}
		if (version > 4) ar& is2D;
	}

public:
	std::string fileName;
	std::string fileGuid;
	bool playOnStart;
	bool loop;
	bool is2D = false;
	float volume = 1.0f;
	float minDistance = 10.0f;
	float maxDistance = 1000.0f;
};

BOOST_CLASS_VERSION(SAudioSource, 5)