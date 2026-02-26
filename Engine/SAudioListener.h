#pragma once

#include "SComponent.h"
#include <string>

class SAudioListener : public SComponent
{
public:
	SAudioListener() {}
	~SAudioListener() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SComponent>(*this);
		if (version > 1) ar& volume;
	}

public:
	float volume = 1.0f;
};

BOOST_CLASS_VERSION(SAudioListener, 2)