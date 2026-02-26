#include "stdafx.h"
#include "API_Component.h"
#include "Engine.h"
#include <Ogre/source/OgreMain/custom/Component.h>

bool API_Component::getEnabled(MonoObject * this_ptr)
{
	Component * component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	return component->GetEnabled();
}

void API_Component::setEnabled(MonoObject * this_ptr, bool enabled)
{
	Component * component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	component->SetEnabled(enabled);
}

MonoObject* API_Component::getGameObject(MonoObject* this_ptr)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	return node->gameObjectMono;
}
