#pragma once

#include "Collider.h"

class MeshCollider : public Collider
{
private:
	bool convex = false;

	void rebuild();

	btCollisionShape * collisionShape = nullptr;

public:
	MeshCollider(SceneNode * parent);
	virtual ~MeshCollider();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	bool GetConvex() { return convex; }
	void SetConvex(bool c);
};

