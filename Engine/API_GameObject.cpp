#include "stdafx.h"
#include "API_GameObject.h"
#include "Engine.h"
#include "RigidBody.h"
#include "AnimationList.h"
#include "AudioSource.h"
#include "NavMeshAgent.h"
#include <OgreSceneNode.h>
#include "StringConverter.h"
#include "ProjectSettings.h"

MonoString * API_GameObject::getName(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	std::string _str = CP_UNI(node->getAlias());
	MonoString * str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), _str.c_str());

	return str;
}

MonoString * API_GameObject::getUniqueName(MonoObject* this_ptr)
{
	SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	std:string _str = CP_UNI(node->getName());
	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), _str.c_str());

	return str;
}

MonoString* API_GameObject::getTag(MonoObject* this_ptr)
{
	SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	auto tags = GetEngine->getProjectSettings()->tags;
	tags.insert(tags.begin(), "Untagged");

	std::string _tag = CP_UNI("Untagged");

	if (node->tag >= 0 && node->tag < tags.size())
		_tag = CP_UNI(tags[node->tag]);

	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), _tag.c_str());

	return str;
}

void API_GameObject::setTag(MonoObject* this_ptr, MonoString* value)
{
	SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	std::string _tag = (const char*)mono_string_to_utf8((MonoString*)value);
	_tag = CP_SYS(_tag);

	auto tags = GetEngine->getProjectSettings()->tags;
	tags.insert(tags.begin(), "Untagged");

	auto it = find(tags.begin(), tags.end(), _tag);

	if (it != tags.end())
		node->tag = distance(tags.begin(), it);
}

MonoString* API_GameObject::getLayer(MonoObject* this_ptr)
{
	SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	auto layers = GetEngine->getProjectSettings()->layers;
	layers.insert(layers.begin(), "Default");

	std::string _layer = CP_UNI("Default");

	if (node->layer >= 0 && node->layer < layers.size())
		_layer = CP_UNI(layers[node->layer]);

	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), _layer.c_str());

	return str;
}

void API_GameObject::setLayer(MonoObject* this_ptr, MonoString* value)
{
	SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	std::string _layer = (const char*)mono_string_to_utf8((MonoString*)value);
	_layer = CP_SYS(_layer);

	auto layers = GetEngine->getProjectSettings()->layers;
	layers.insert(layers.begin(), "Default");

	auto it = find(layers.begin(), layers.end(), _layer);

	if (it != layers.end())
		node->layer = distance(layers.begin(), it);
}

bool API_GameObject::getEnabled(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	return node->getGlobalVisible();
}

void API_GameObject::setEnabled(MonoObject * this_ptr, bool visible)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	if (node->getVisible() != visible)
	{
		bool prevVisible = node->getVisible();
		node->setVisible(visible, true);

		if (prevVisible == false && visible && ((SceneNode*)node->getParent())->getVisible())
		{
			for (auto it = node->monoScripts.begin(); it != node->monoScripts.end(); ++it)
			{
				if ((*it)->enabled)
				{
					if (!(*it)->startExecuted)
					{
						(*it)->startExecuted = true;
						GetEngine->GetMonoRuntime()->Execute((MonoObject*)(*it)->object, "Start");
					}
				}
			}
		}
	}
}

MonoObject* API_GameObject::getAttachedObject(MonoObject* this_ptr)
{
	SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	if (node->getAttachedObjects().size() > 0)
	{
		MovableObject* obj = node->getAttachedObject(0);

		return obj->objectMono;
	}

	return nullptr;
}

MonoObject * API_GameObject::getTransform(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	MonoObject * transform = node->transformMono;

	return transform;
}

MonoObject * API_GameObject::getRigidbody(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody * body = ((RigidBody*)node->GetComponent(RigidBody::COMPONENT_TYPE));
	MonoObject * rigidbody = nullptr;

	if (body != nullptr)
		rigidbody = body->GetMonoObject();

	return rigidbody;
}

MonoObject * API_GameObject::getAnimation(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	AnimationList * animList = ((AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE));
	MonoObject * animation = nullptr;
	
	if (animList != nullptr)
		animation = animList->GetMonoObject();

	return animation;
}

MonoObject * API_GameObject::getAudioSource(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	AudioSource * audio = ((AudioSource*)node->GetComponent(AudioSource::COMPONENT_TYPE));
	MonoObject * audioSource = nullptr;

	if (audio != nullptr)
		audioSource = audio->GetMonoObject();

	return audioSource;
}

MonoObject * API_GameObject::getNavMeshAgent(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent * agent = ((NavMeshAgent*)node->GetComponent(NavMeshAgent::COMPONENT_TYPE));
	MonoObject * navMeshAgent = nullptr;

	if (agent != nullptr)
		navMeshAgent = agent->GetMonoObject();

	return navMeshAgent;
}

MonoObject * API_GameObject::findGameObject(MonoObject * name)
{
	std::string _name = (const char*)mono_string_to_utf8((MonoString*)name);
	_name = CP_SYS(_name);

	SceneNode * node = GetEngine->GetSceneManager()->getSceneNodeByAlias(_name);
	if (node != nullptr)
		return node->gameObjectMono;

	return nullptr;
}

MonoObject * API_GameObject::getGameObject(MonoObject* name)
{
	std::string _name = (const char*)mono_string_to_utf8((MonoString*)name);
	_name = CP_SYS(_name);

	if (GetEngine->GetSceneManager()->hasSceneNode(_name))
	{
		SceneNode* node = GetEngine->GetSceneManager()->getSceneNode(_name);
		return node->gameObjectMono;
	}

	return nullptr;
}

MonoObject * API_GameObject::getMonoBehaviour(MonoObject * this_ptr, MonoObject * className)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	std::string _className = (const char*)mono_string_to_utf8((MonoString*)className);
	_className = CP_SYS(_className);
	MonoClass* _class = GetEngine->GetMonoRuntime()->FindClass(_className);

	for (std::vector<MonoScript*>::iterator it = node->monoScripts.begin(); it != node->monoScripts.end(); ++it)
	{
		if ((*it)->_class == _class)
			return (MonoObject*)(*it)->object;
	}

	return nullptr;
}

MonoObject * API_GameObject::getMonoBehaviourT(MonoObject * this_ptr, MonoReflectionType * type)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	MonoType * _type = mono_reflection_type_get_type(type);
	MonoClass * _class = mono_type_get_class(_type);
	//std::string _className = mono_class_get_name(_class);
	//_className = CP_SYS(_className);

	for (std::vector<MonoScript*>::iterator it = node->monoScripts.begin(); it != node->monoScripts.end(); ++it)
	{
		if ((*it)->_class == _class)
			return (MonoObject*)(*it)->object;
	}

	return nullptr;
}

MonoObject * API_GameObject::getComponentT(MonoObject * this_ptr, MonoReflectionType * type)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	MonoType * _type = mono_reflection_type_get_type(type);
	MonoClass * _class = mono_type_get_class(_type);
	std::string _className = mono_class_get_name(_class);
	_className = CP_SYS(_className);

	/*if (_className == "Animation")
		_className = "AnimationList";*/

	Component * component = node->GetComponent(_className);
	if (component != nullptr)
		return component->GetMonoObject();

	return nullptr;
}
