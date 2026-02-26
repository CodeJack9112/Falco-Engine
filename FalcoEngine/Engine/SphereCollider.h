#pragma once

#include "Collider.h"

class btSphereShape;

class SphereCollider : public Collider
{
private:
	float radius = 1;

	btSphereShape * collisionShape = nullptr;

public:
	SphereCollider(SceneNode * parent);
	virtual ~SphereCollider();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	float GetRadius() { return radius; }
	void SetRadius(float value);
};

