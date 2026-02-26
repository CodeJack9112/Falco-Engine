#pragma once

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "PhysicsManager.h"
#include <string>

using namespace std;

class FixedJoint : public Component
{
private:
	std::string connectedObjectName = "[None]";
	Vector3 anchor = Vector3(0, 0, 0);
	Vector3 connectedAnchor = Vector3(0, 0, 0);

	bool linkedBodiesCollision = true;

	btFixedConstraint* fixedConstraint = nullptr;

public:
	FixedJoint(SceneNode* parent);
	virtual ~FixedJoint();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	std::string GetConnectedObjectName() { return connectedObjectName; }
	void SetConnectedObjectName(std::string name);

	Vector3 GetAnchor() { return anchor; }
	void SetAnchor(Vector3 value) { anchor = value; }

	Vector3 GetConnectedAnchor() { return connectedAnchor; }
	void SetConnectedAnchor(Vector3 value) { connectedAnchor = value; }

	bool GetLinkedBodiesCollision() { return linkedBodiesCollision; }
	void SetLinkedBodiesCollision(bool value) { linkedBodiesCollision = value; }

	void AutoConfigureAnchors();

	virtual void SceneLoaded();
	virtual void StateChanged(bool active);
	virtual void NodeStateChanged(bool active);
};

