#pragma once

struct SUIEvent
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & sceneNode;
		ar & functionName;
	}

	SUIEvent() {}
	~SUIEvent() {}

	std::string sceneNode = "None";
	std::string functionName = "None";
};

BOOST_CLASS_VERSION(SUIEvent, 1)