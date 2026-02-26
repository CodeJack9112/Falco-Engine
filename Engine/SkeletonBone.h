#pragma once

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Ogre/source/OgreMain/include/OgreVector3.h"
#include "../Ogre/source/OgreMain/include/OgreQuaternion.h"
#include <string>

using namespace std;

namespace fbxsdk
{
	class FbxNode;
}

using namespace fbxsdk;

class SkeletonBone : public Component
{
private:
	int index = -1;
	int handle = -1;
	std::string name;
	Vector3 bindingPosition = Vector3(0, 0, 0);
	Vector3 position = Vector3(0, 0, 0);
	Vector3 scale = Vector3(1, 1, 1);
	Vector3 bindingScale = Vector3(1, 1, 1);
	Quaternion bindingRotation = Quaternion::IDENTITY;
	Quaternion rotation = Quaternion::IDENTITY;
	std::string modelFileName = "";
	std::string modelFileGuid = "";
	std::string rootNodeName = "";
	bool isRootBone = false;

public:
	SkeletonBone(SceneNode * parent);
	virtual ~SkeletonBone();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

public:
	int GetIndex() { return index; }
	void SetIndex(int i) { index = i; };

	std::string GetName() { return name; }
	void SetName(std::string _name) { name = _name; }

	Vector3 GetPosition() { return position; }
	void SetPosition(Vector3 pos) { position = pos; }

	Vector3 GetScale() { return scale; }
	void SetScale(Vector3 scl) { scale = scl; }

	Quaternion GetRotation() { return rotation; }
	void SetRotation(Quaternion rot) { rotation = rot; }

	Vector3 GetBindingPosition() { return bindingPosition; }
	void SetBindingPosition(Vector3 pos) { bindingPosition = pos; }

	Vector3 GetBindingScale() { return bindingScale; }
	void SetBindingScale(Vector3 scl) { bindingScale = scl; }

	Quaternion GetBindingRotation() { return bindingRotation; }
	void SetBindingRotation(Quaternion rot) { bindingRotation = rot; }

	std::string GetModelFileName() { return modelFileName; }
	void SetModelFileName(std::string name);

	std::string GetModelFileGuid() { return modelFileGuid; }

	std::string GetRootNodeName() { return rootNodeName; }
	void SetRootNodeName(std::string name) { rootNodeName = name; }

	int GetHandle() { return handle; }
	void SetHandle(int h) { handle = h; }

	bool GetIsRootBone() { return isRootBone; }
	void SetIsRootBone(bool value) { isRootBone = value; }

	void CreateGizmos();
};

