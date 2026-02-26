#pragma once

#include "Collider.h"

class btBoxShape;

class BoxCollider : public Collider
{
private:
	Vector3 boxSize = Vector3(1, 1, 1);

	btBoxShape * collisionShape = nullptr;

public:
	BoxCollider(SceneNode * parent);
	virtual ~BoxCollider();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	Vector3 GetBoxSize() { return boxSize; }
	void SetBoxSize(Vector3 value);
};

