#pragma once

struct SNavMeshSettings
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & walkableSlopeAngle;
		ar & walkableHeight;
		ar & walkableClimb;
		ar & walkableRadius;
		ar & maxEdgeLen;
		ar & maxSimplificationError;
		ar & minRegionArea;
		ar & mergeRegionArea;
		ar & cellSize;
		ar & cellHeight;
	}

	SNavMeshSettings() {}
	~SNavMeshSettings() {}

	float walkableSlopeAngle = 45;
	float walkableHeight = 2.0;
	float walkableClimb = 3.9;
	float walkableRadius = 1.6;
	float maxEdgeLen = 12;
	float maxSimplificationError = 3.0;
	float minRegionArea = 4.0;
	float mergeRegionArea = 20.0;
	float cellSize = 1.00;
	float cellHeight = 0.20;
};

BOOST_CLASS_VERSION(SNavMeshSettings, 1)