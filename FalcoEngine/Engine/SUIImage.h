#pragma once

#include <string>
#include "SUIElement.h"

class SUIImage : public SUIElement
{
public:
	SUIImage() = default;
	~SUIImage() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SUIElement>(*this);
		ar & image;
		if (version > 1) ar& imageGuid;
	}

public:
	std::string image;
	std::string imageGuid;
};

BOOST_CLASS_VERSION(SUIImage, 2)