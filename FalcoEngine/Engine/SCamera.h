#pragma once

#include "SSceneNode.h"
#include "SVector.h"

class SCamera : public SSceneNode
{
public:
	SCamera() = default;
	~SCamera() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SSceneNode>(*this);
		ar & nearClipPlane;
		ar & farClipPlane;
		ar & fieldOfView;
		ar & isMainCamera;
		if (version > 1) ar& clearColor;
		if (version > 2)
		{
			ar& orthographic;
			ar& orthoSize;
		}
	}

	float nearClipPlane;
	float farClipPlane;
	float fieldOfView;
	bool isMainCamera;
	bool orthographic = false;
	float orthoSize = 1.0f;
	SColor clearColor;
};

BOOST_CLASS_VERSION(SCamera, 3)

