#pragma once

#include "SComponent.h"
#include <string>

class SNavMeshObstacle : public SComponent
{
public:
	SNavMeshObstacle() {}
	~SNavMeshObstacle() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::base_object<SComponent>(*this);
		ar& size;
		ar& offset;
	}

public:
	SVector3 size = SVector3(1, 1, 1);
	SVector3 offset = SVector3(0, 0, 0);
};

BOOST_CLASS_VERSION(SNavMeshObstacle, 1)