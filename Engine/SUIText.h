#pragma once

#include <string>
#include "SUIElement.h"

class SUIText : public SUIElement
{
public:
	SUIText() = default;
	~SUIText() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SUIElement>(*this);
		ar & font;
		ar & text;
		ar & color;
		ar & size;
		if (version > 1) ar & fontGuid;
		if (version > 2)
		{
			ar& verticalAlignment;
			ar& horizontalAlignment;
		}
	}

public:
	std::string font;
	std::string fontGuid;
	std::string text;
	SColor color;
	float size;
	int verticalAlignment;
	int horizontalAlignment;
};

BOOST_CLASS_VERSION(SUIText, 3)