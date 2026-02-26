#pragma once

struct SGrassDataBrush
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& materialPath;
		ar& minSize;
		ar& maxSize;
		ar& animation;
		ar& swayDistribution;
		ar& swayLength;
		ar& swaySpeed;
		if (version > 1) ar& density;
		if (version > 2)
		{
			for (int i = 0; i < 129; ++i)
			{
				for (int j = 0; j < 129; ++j)
					ar& densityMap[i][j];
			}
		}
		if (version > 3) ar& materialGuid;
	}

	SGrassDataBrush() {}
	~SGrassDataBrush() {}

	std::string materialPath;
	std::string materialGuid;
	SVector2 minSize;
	SVector2 maxSize;
	bool animation = true;
	float swayDistribution = 1;
	float swayLength = 1;
	float swaySpeed = 1;
	float density = 1;
	uint8 densityMap[129][129];
};

BOOST_CLASS_VERSION(SGrassDataBrush, 4)