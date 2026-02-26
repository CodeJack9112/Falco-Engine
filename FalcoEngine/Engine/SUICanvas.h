#pragma once

#include "SSceneNode.h"
#include <string>

class SUICanvas : public SSceneNode
{
public:
	SUICanvas() = default;
	~SUICanvas() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SSceneNode>(*this);
		ar & refScreenWidth;
		ar & refScreenHeight;
		ar & scaleMode;
	}

public:
	int refScreenWidth = 0;
	int refScreenHeight = 0;
	int scaleMode = 0;
};

BOOST_CLASS_VERSION(SUICanvas, 1)