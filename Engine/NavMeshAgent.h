#pragma once

#include <string>
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Mono/include/mono/metadata/object.h"
#include "../Ogre/source/OgreMain/include/OgreVector3.h"

#define MAX_PATHSLOT		1 // how many paths we can store
#define MAX_PATHPOLY		2048 // max number of polygons in a path
#define MAX_PATHVERT		2048 // max verts in a path 

class DynamicLines;
typedef unsigned int dtPolyRef;

struct PathData
{
	float PosX[MAX_PATHVERT];
	float PosY[MAX_PATHVERT];
	float PosZ[MAX_PATHVERT];
	int VertCount = 0;
	bool valid = true;
};

class NavMeshAgent : public Component
{
private:
	float radius = 0.6;
	float height = 2.0;
	float speed = 3.5;
	float acceleration = 8.0;
	float rotationSpeed = 1.0;

	Vector3 targetPosition = Vector3::ZERO;

	PathData FindPath(Vector3 StartPos, Vector3 EndPos);
	void UpdateParams();

	DynamicLines * debugPath = nullptr;
	PathData currentPath;

public:
	NavMeshAgent(SceneNode * parent);
	virtual ~NavMeshAgent();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	void Update();

	void SetTargetPosition(Vector3 pos);
	Vector3 GetTargetPosition() { return targetPosition; }

	float GetRadius() { return radius; }
	void SetRadius(float r);

	float GetHeight() { return height; }
	void SetHeight(float h);

	float GetSpeed() { return speed; }
	void SetSpeed(float s);

	float GetAcceleration() { return acceleration; }
	void SetAcceleration(float a);

	float GetRotationSpeed() { return rotationSpeed; }
	void SetRotationSpeed(float s);

	void Init();
};

