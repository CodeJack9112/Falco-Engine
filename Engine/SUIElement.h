#pragma once

#include "SSceneNode.h"
#include <string>

class SUIElement : public SSceneNode
{
public:
	SUIElement() = default;
	~SUIElement() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SSceneNode>(*this);
		ar & canvasAlignment;
		ar & anchor;

		if (version > 1) ar & color;
		if (version > 2) ar & elementSize;
	}

public:
	int canvasAlignment = 0;
	SVector2 anchor = SVector2(0.5, 0.5);
	SVector2 elementSize = SVector2(1.0, 1.0);
	SColor color = SColor(1.0, 1.0, 1.0, 1.0);
};

BOOST_CLASS_VERSION(SUIElement, 3)