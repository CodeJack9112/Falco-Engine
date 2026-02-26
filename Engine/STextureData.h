#pragma once

struct STextureData
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& diffusePath;
		ar& diffuseGuid;
		ar& normalPath;
		ar& normalGuid;
		ar& worldSize;
	}

	STextureData() {}
	~STextureData() {}

	std::string diffusePath = "";
	std::string diffuseGuid = "";
	std::string normalPath = "";
	std::string normalGuid = "";
	float worldSize = 100.0f;
};

BOOST_CLASS_VERSION(STextureData, 1)