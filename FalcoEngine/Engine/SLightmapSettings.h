#pragma once

struct SLightmapSettings
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& defaultLightmapSize;
		ar& lightmapBlurRadius;

		if (version > 1)
			ar& regionLightmapSize;
	}

	SLightmapSettings() {}
	~SLightmapSettings() {}

	int defaultLightmapSize = 256;
	int regionLightmapSize = 1024;
	float lightmapBlurRadius = 0.5f;
};

BOOST_CLASS_VERSION(SLightmapSettings, 2)