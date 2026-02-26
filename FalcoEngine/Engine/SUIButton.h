#pragma once

#include <string>
#include "SUIElement.h"
#include "SUIEvent.h"

class SUIButton : public SUIElement
{
public:
	SUIButton() = default;
	~SUIButton() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SUIElement>(*this);
		ar & imageNormal;
		ar & imageHover;
		ar & imagePressed;
		ar & imageDisabled;
		ar & pointerDownEvent;
		ar & pointerUpEvent;
		if (version > 1)
		{
			ar& imageNormalGuid;
			ar& imageHoverGuid;
			ar& imagePressedGuid;
			ar& imageDisabledGuid;
		}
		if (version > 2)
		{
			ar& colorNormal;
			ar& colorHover;
			ar& colorPressed;
			ar& colorDisabled;
			ar& interactable;
		}
	}

public:
	std::string imageNormal;
	std::string imageHover;
	std::string imagePressed;
	std::string imageDisabled;
	std::string imageNormalGuid;
	std::string imageHoverGuid;
	std::string imagePressedGuid;
	std::string imageDisabledGuid;
	SColor colorNormal = SColor(1, 1, 1, 1);
	SColor colorHover = SColor(1, 1, 1, 1);
	SColor colorPressed = SColor(1, 1, 1, 1);
	SColor colorDisabled = SColor(1, 1, 1, 1);
	SUIEvent pointerDownEvent;
	SUIEvent pointerUpEvent;
	bool interactable = true;
};

BOOST_CLASS_VERSION(SUIButton, 3)