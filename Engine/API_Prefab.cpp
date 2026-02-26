#include "stdafx.h"
#include "API_Prefab.h"
#include "Engine.h"
#include "SceneSerializer.h"
#include "StringConverter.h"
#include "RigidBody.h"

#include <OgreSceneNode.h>

MonoObject * API_Prefab::instantiate(MonoObject * prefabFileName, API::Vector3 * position, API::Quaternion * rotation)
{
	std::string _path = (const char*)mono_string_to_utf8((MonoString*)prefabFileName);
	_path = CP_SYS(_path);

	std::string path = GetEngine->GetAssetsPath() + _path;

	SceneSerializer serializer;
	SceneNode * node = serializer.DeserializeFromPrefab(GetEngine->GetSceneManager(), path);

	Vector3 _pos = Vector3(position->x, position->y, position->z);
	Quaternion _rot = Quaternion(rotation->w, rotation->x, rotation->y, rotation->z);

	node->_setDerivedPosition(_pos);
	node->_setDerivedOrientation(_rot);

	RigidBody* rb = (RigidBody*)node->GetComponent(RigidBody::COMPONENT_TYPE);
	if (rb != nullptr)
	{
		rb->SetPosition(_pos);
		rb->SetRotation(_rot);
	}

	return node->gameObjectMono;
}
