#pragma once

#include "Collider.h"

class btCapsuleShape;

class CapsuleCollider : public Collider
{
private:
	float radius = 1.0;
	float height = 2.0;
	
	btCapsuleShape * collisionShape = nullptr;

public:
	CapsuleCollider(SceneNode * parent);
	virtual ~CapsuleCollider();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	float GetRadius() { return radius; }
	float GetHeight() { return height; }

	void SetRadius(float r);
	void SetHeight(float h);
};

