#include "API_AttachedObject.h"
#include "OgreSceneNode.h"
#include "OgreMovableObject.h"
#include "Engine.h"

MonoObject* API_AttachedObject::getGameObject(MonoObject* this_ptr)
{
	MovableObject* obj;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&obj));

	if (obj != nullptr)
	{
		SceneNode* node = obj->getParentSceneNode();

		if (node != nullptr)
			return node->gameObjectMono;
	}

	return nullptr;
}
