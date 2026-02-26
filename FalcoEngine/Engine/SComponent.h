#pragma once

#include <string>

class SComponent
{
public:
	SComponent() {}
	~SComponent() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & enabled;
	}

public:
	bool enabled = true;
};

BOOST_CLASS_VERSION(SComponent, 1)