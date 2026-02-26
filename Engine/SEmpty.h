#pragma once

#include "SSceneNode.h"
#include <string>

class SEmpty : public SSceneNode
{
public:
	SEmpty() = default;
	~SEmpty() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SSceneNode>(*this);
	}

public:
	
};

BOOST_CLASS_VERSION(SEmpty, 1)