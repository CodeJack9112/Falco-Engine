#pragma once

#include "SMonoFieldInfo.h"
#include "../boost/serialization/vector.hpp"

class SMonoScript
{
public:
	SMonoScript() = default;
	~SMonoScript() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & className;
		ar & BOOST_SERIALIZATION_NVP(monoFieldList);
		if (version > 1) ar & enabled;
	}

public:
	std::string className;
	bool enabled = true;
	std::vector<SMonoFieldInfo> monoFieldList;
};

BOOST_CLASS_VERSION(SMonoScript, 2)