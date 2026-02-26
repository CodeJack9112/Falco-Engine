#pragma once

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "MeshUtilities.h"
#include "PhysicsManager.h"
#include <string>

using namespace std;

class Collider : public Component
{
private:
	btCollisionShape * collisionShape = nullptr;
	Vector3 offset = Vector3(0, 0, 0);
	Quaternion rotation = Quaternion::IDENTITY;
	bool isTrigger = false;

	void reloadBody();

public:
	Collider(SceneNode * parent, MonoClass* monoClass = nullptr) : Component(parent, monoClass) {}
	virtual ~Collider();

	void SetCollisionShape(btCollisionShape* shape);
	btCollisionShape * GetCollisionShape() { return collisionShape; }

	Vector3 GetOffset() { return offset; }
	void SetOffset(Vector3 value);

	Quaternion GetRotation() { return rotation; }
	void SetRotation(Quaternion value);

	bool GetIsTrigger() { return isTrigger; }
	void SetIsTrigger(bool value);

	virtual void StateChanged(bool active);
	virtual void NodeStateChanged(bool active);
};