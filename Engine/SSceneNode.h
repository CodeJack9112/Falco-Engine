#pragma once

#include <string>
#include "SVector.h"
#include "SQuaternion.h"
#include "SMonoScript.h"
#include "SComponent.h"
#include "SAudioSource.h"
#include "SAudioListener.h"
#include "SRigidBody.h"
#include "SMeshCollider.h"
#include "SCapsuleCollider.h"
#include "SBoxCollider.h"
#include "SSphereCollider.h"
#include "SSkeletonBone.h"
#include "SAnimationList.h"
#include "SHingeJoint.h"
#include "SFixedJoint.h"
#include "SCharacterJoint.h"
#include "SNavMeshAgent.h"
#include "SNavMeshObstacle.h"
#include "SVehicle.h"

enum ObjectType
{
	OT_Entity,
	OT_Light,
	OT_Empty,
	OT_Camera,
	OT_UICanvas,
	OT_UIButton,
	OT_UIText,
	OT_UIImage,
	OT_ParticleSystem
};

class SSceneNode
{
public:
	SSceneNode() {}
	~SSceneNode() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & index;
		ar & name;
		ar & parentName;
		ar & type;
		ar & position;
		ar & rotation;
		ar & scale;

		ar & BOOST_SERIALIZATION_NVP(monoScripts);
		ar & BOOST_SERIALIZATION_NVP(audioSourceComponents);
		ar & BOOST_SERIALIZATION_NVP(audioListenerComponents);
		ar & BOOST_SERIALIZATION_NVP(rigidbodyComponents);
		ar & BOOST_SERIALIZATION_NVP(meshColliderComponents);
		ar & BOOST_SERIALIZATION_NVP(capsuleColliderComponents);
		ar & BOOST_SERIALIZATION_NVP(skeletonBoneComponents);
		ar & BOOST_SERIALIZATION_NVP(animationListComponents);
		ar & BOOST_SERIALIZATION_NVP(hingeJointComponents);
		ar & BOOST_SERIALIZATION_NVP(navMeshAgentComponents);
		if (version > 1) ar & BOOST_SERIALIZATION_NVP(boxColliderComponents);
		if (version > 2) ar & BOOST_SERIALIZATION_NVP(sphereColliderComponents);
		if (version > 3) ar & BOOST_SERIALIZATION_NVP(characterJointComponents);
		
		ar & fbxSkeletonFrom;
		ar & fbxSkeletonFromFile;
		ar & fbxCurvesFrom;
		ar & fbxCurvesFromFile;

		if (version > 4)
		{
			ar& lightmapStatic;
			ar& navigationStatic;
		}

		if (version > 5) ar& alias;
		if (version > 6)
		{
			ar& visible;
			ar& enabled;
		}

		if (version > 7)
		{
			ar& fbxSkeletonFromGuid;
			ar& fbxCurvesFromGuid;
		}

		if (version > 8)
		{
			ar& prefabName;
			ar& prefabGuid;
		}

		if (version > 9)
			ar& skinned;

		if (version > 10)
		{
			ar& fbxFromFile;
			ar& fbxFromGuid;
		}

		if (version > 11)
		{
			ar& staticBatching;
		}

		if (version > 12)
		{
			ar& tag;
			ar& layer;
		}

		if (version > 13) ar& BOOST_SERIALIZATION_NVP(navMeshObstacleComponents);
		if (version > 15)
		{
			ar& BOOST_SERIALIZATION_NVP(fixedJointComponents);
			ar& BOOST_SERIALIZATION_NVP(vehicleComponents);
		}
	}

public:
	int index = 0;
	std::string prefabName = "";
	std::string prefabGuid = "";

	int fbxSkeletonFrom = -1;
	std::string fbxSkeletonFromFile = "";
	std::string fbxSkeletonFromGuid = "";

	int fbxCurvesFrom = -1;
	std::string fbxCurvesFromFile = "";
	std::string fbxCurvesFromGuid = "";

	std::string fbxFromFile = "";
	std::string fbxFromGuid = "";

	std::string name = "";
	std::string alias = "";

	std::string tag = "0";
	std::string layer = "0";

	bool lightmapStatic = false;
	bool navigationStatic = false;
	bool staticBatching = false;
	bool visible = true;
	bool enabled = true;
	bool skinned = false;

	std::string parentName;
	ObjectType type;

	SVector3 position;
	SQuaternion rotation;
	SVector3 scale;

	std::vector<SMonoScript> monoScripts;
	std::vector<SAudioSource> audioSourceComponents;
	std::vector<SAudioListener> audioListenerComponents;
	std::vector<SRigidBody> rigidbodyComponents;
	std::vector<SMeshCollider> meshColliderComponents;
	std::vector<SCapsuleCollider> capsuleColliderComponents;
	std::vector<SSkeletonBone> skeletonBoneComponents;
	std::vector<SAnimationList> animationListComponents;
	std::vector<SHingeJoint> hingeJointComponents;
	std::vector<SFixedJoint> fixedJointComponents;
	std::vector<SCharacterJoint> characterJointComponents;
	std::vector<SNavMeshAgent> navMeshAgentComponents;
	std::vector<SBoxCollider> boxColliderComponents;
	std::vector<SSphereCollider> sphereColliderComponents;
	std::vector<SNavMeshObstacle> navMeshObstacleComponents;
	std::vector<SVehicle> vehicleComponents;
};

BOOST_CLASS_VERSION(SSceneNode, 16)