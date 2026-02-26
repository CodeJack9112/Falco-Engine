#pragma once

#include <string>
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Mono/include/mono/metadata/object.h"
#include <OgreVector3.h>

class NavMeshObstacle : public Component
{
public:
	NavMeshObstacle(SceneNode* parent);
	virtual ~NavMeshObstacle();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	Ogre::Vector3 getSize() { return size; }
	void setSize(Ogre::Vector3 value) { size = value; }

	Ogre::Vector3 getOffset() { return offset; }
	void setOffset(Ogre::Vector3 value) { offset = value; }

	virtual void SceneLoaded();
	virtual void StateChanged(bool active);
	virtual void NodeStateChanged(bool active);

	void update();

private:
	Ogre::Vector3 size = Ogre::Vector3(1.0f, 1.0f, 1.0f);
	Ogre::Vector3 offset = Ogre::Vector3::ZERO;

	unsigned int obstacleRef = 0;

	Vector3 oldPos = Vector3::ZERO;
	Quaternion oldRot = Quaternion::IDENTITY;
	Vector3 oldScale = Vector3(1, 1, 1);
	Vector3 oldSize = Vector3(1, 1, 1);
	Vector3 oldOffset = Vector3(1, 1, 1);

	void addObstacle();
	void removeObstacle();
	void updateObstacle();
};

