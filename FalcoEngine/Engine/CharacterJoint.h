#pragma once

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "PhysicsManager.h"
#include <string>

using namespace std;

class CharacterJoint : public Component
{
private:
	std::string connectedObjectName = "[None]";
	Vector3 anchor = Vector3(0, 0, 0);
	Vector3 connectedAnchor = Vector3(0, 0, 0);
	Vector3 limitMin = Vector3(-3.14, -3.14, -3.14);
	Vector3 limitMax = Vector3(3.14, 3.14, 3.14);
	Vector3 axis = Vector3(1, 1, 1);
	Vector3 linearAxis = Vector3(1, 1, 1);
	bool linkedBodiesCollision = true;

	btGeneric6DofConstraint * characterConstraint = nullptr;

public:
	CharacterJoint(SceneNode * parent);
	virtual ~CharacterJoint();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	std::string GetConnectedObjectName() { return connectedObjectName; }
	void SetConnectedObjectName(std::string name);

	Vector3 GetAnchor() { return anchor; }
	void SetAnchor(Vector3 value) { anchor = value; }

	Vector3 GetConnectedAnchor() { return connectedAnchor; }
	void SetConnectedAnchor(Vector3 value) { connectedAnchor = value; }

	Vector3 GetLimitMin() { return limitMin; }
	void SetLimitMin(Vector3 value) { limitMin = value; }

	Vector3 GetLimitMax() { return limitMax; }
	void SetLimitMax(Vector3 value) { limitMax = value; }

	Vector3 GetAxis() { return axis; }
	void SetAxis(Vector3 value) { axis = value; }

	Vector3 GetLinearAxis() { return axis; }
	void SetLinearAxis(Vector3 value) { axis = value; }

	bool GetLinkedBodiesCollision() { return linkedBodiesCollision; }
	void SetLinkedBodiesCollision(bool value) { linkedBodiesCollision = value; }

	void AutoConfigureAnchors();

	virtual void SceneLoaded();
	virtual void StateChanged(bool active);
	virtual void NodeStateChanged(bool active);
};

