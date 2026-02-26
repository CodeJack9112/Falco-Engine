#include "stdafx.h"
#include "API_MonoBehaviour.h"
#include "RigidBody.h"
#include "AnimationList.h"
#include "AudioSource.h"
#include "NavMeshAgent.h"
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "StringConverter.h"
#include "PostEffectListener.h"
#include "OgreCompositorManager.h"
#include "OgreCamera.h"
#include "OgreCompositorInstance.h"
#include "OgreCompositor.h"

void API_MonoBehaviour::dctor(MonoObject* this_ptr)
{
	MonoClass * _class = mono_object_get_class(this_ptr);
	
	//Destroy post effects
	if (mono_class_is_subclass_of(_class, GetEngine->GetMonoRuntime()->posteffect_class, false))
	{
		Compositor * comp;
		CompositorInstance* compInstance = nullptr;
		PostEffectListener* listener = nullptr;
		mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_this_ptr_field, reinterpret_cast<void*>(&comp));
		mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_instance_this_ptr_field, reinterpret_cast<void*>(&compInstance));
		mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_listener_ptr, reinterpret_cast<void*>(&listener));

		if (compInstance != nullptr)
		{
			if (listener != nullptr)
			{
				compInstance->removeListener(listener);
				delete listener;
			}

			SceneNode* node;
			mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

			if (node->getAttachedObjects().size() > 0)
			{
				if (node->getAttachedObject(0)->getMovableType() == "Camera")
				{
					Camera* camera = (Camera*)node->getAttachedObject(0);
					compInstance->setEnabled(false);
					CompositorManager::getSingleton().removeCompositor(camera->getViewport(), comp->getName());
					CompositorManager::getSingleton().remove(comp->getHandle());
				}
			}
		}
	}
}

MonoString * API_MonoBehaviour::getName(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(node->getAlias()).c_str());

	return str;
}

MonoString * API_MonoBehaviour::getUniqueName(MonoObject* this_ptr)
{
	SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(node->getName()).c_str());

	return str;
}

bool API_MonoBehaviour::getEnabled(MonoObject * this_ptr)
{
	MonoScript* script = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->script_ptr_field, reinterpret_cast<void*>(&script));

	if (script != nullptr)
	{
		return script->enabled;
	}

	return false;
}

void API_MonoBehaviour::setEnabled(MonoObject * this_ptr, bool enabled)
{
	API::APIFuncs::setEnabled(this_ptr, enabled);
}

MonoObject * API_MonoBehaviour::getTransform(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	MonoObject * transform = node->transformMono;

	return transform;
}

MonoObject * API_MonoBehaviour::getRigidbody(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	MonoObject * rigidbody = ((RigidBody*)node->GetComponent(RigidBody::COMPONENT_TYPE))->GetMonoObject();

	return rigidbody;
}

MonoObject * API_MonoBehaviour::getGameObject(MonoObject * this_ptr)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	return node->gameObjectMono;
}

MonoObject * API_MonoBehaviour::getAnimation(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	AnimationList* animList = ((AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE));
	if (animList != nullptr)
		return animList->GetMonoObject();

	return nullptr;
}

MonoObject * API_MonoBehaviour::getAudioSource(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	AudioSource * audio = ((AudioSource*)node->GetComponent(AudioSource::COMPONENT_TYPE));
	MonoObject * audioSource = nullptr;

	if (audio != nullptr)
		audioSource = audio->GetMonoObject();

	return audioSource;
}

MonoObject * API_MonoBehaviour::getNavMeshAgent(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent * agent = ((NavMeshAgent*)node->GetComponent(NavMeshAgent::COMPONENT_TYPE));
	MonoObject * navMeshAgent = nullptr;

	if (agent != nullptr)
		navMeshAgent = agent->GetMonoObject();

	return navMeshAgent;
}

MonoObject * API_MonoBehaviour::getMonoBehaviour(MonoObject * this_ptr, MonoObject * className)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	std::string _className = (const char*)mono_string_to_utf8((MonoString*)className);
	_className = CP_SYS(_className);
	MonoClass* _class = GetEngine->GetMonoRuntime()->FindClass(_className);

	for (std::vector<MonoScript*>::iterator it = node->monoScripts.begin(); it != node->monoScripts.end(); ++it)
	{
		bool isSubClass = mono_class_is_subclass_of((*it)->_class, _class, true);

		if ((*it)->_class == _class || isSubClass)
			return (MonoObject*)(*it)->object;
	}

	return nullptr;
}

MonoObject * API_MonoBehaviour::getMonoBehaviourT(MonoObject * this_ptr, MonoReflectionType * type)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	MonoType * _type = mono_reflection_type_get_type(type);
	MonoClass * _class = mono_type_get_class(_type);

	for (std::vector<MonoScript*>::iterator it = node->monoScripts.begin(); it != node->monoScripts.end(); ++it)
	{
		bool isSubClass = mono_class_is_subclass_of((*it)->_class, _class, true);

		if ((*it)->_class == _class || isSubClass)
			return (MonoObject*)(*it)->object;
	}

	return nullptr;
}

MonoObject * API_MonoBehaviour::getComponentT(MonoObject * this_ptr, MonoReflectionType * type)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

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

void API_MonoBehaviour::destroyGameObject(MonoObject * this_ptr, MonoObject * gameObject)
{
	SceneNode * node = nullptr;
	//mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));
	mono_field_get_value(gameObject, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	if (node != nullptr)
	{
		GetEngine->GetMonoRuntime()->AddDestroyObject(node);
		//GetEngine->Destroy(node);
	}
}

void API_MonoBehaviour::destroyComponent(MonoObject * this_ptr, MonoObject * component)
{
	SceneNode * node = nullptr;
	//mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	Component* comp;
	mono_field_get_value(component, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&comp));
	mono_field_get_value(component, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	if (node != nullptr)
	{
		//Component * comp = node->GetComponent(component);

		if (comp != nullptr)
			node->RemoveComponent(comp);
	}
}

void searchTypeRecursive(SceneNode* root, MonoClass* _class, MonoObject*& outType, bool findInactive)
{
	std::vector<SceneNode*> nstack;

	auto children = root->getChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
		nstack.push_back((SceneNode*)*it);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////

		if (outType != nullptr)
			break;

		if (!findInactive)
		{
			if (!curNode->getVisible())
				continue;
		}

		for (auto _it = curNode->components.begin(); _it != curNode->components.end(); ++_it)
		{
			Component* comp = *_it;

			if (!findInactive)
			{
				if (!comp->GetEnabled())
					continue;
			}

			if (comp->GetMonoObject() == nullptr)
				continue;

			MonoClass* cls = mono_object_get_class(comp->GetMonoObject());
			bool isSubClass = mono_class_is_subclass_of(cls, _class, true);

			if (cls == _class || isSubClass)
			{
				outType = comp->GetMonoObject();
				break;
			}
		}

		if (outType == nullptr)
		{
			for (auto _it = curNode->monoScripts.begin(); _it != curNode->monoScripts.end(); ++_it)
			{
				MonoScript* script = *_it;
				if (!findInactive)
				{
					if (!script->enabled)
						continue;
				}

				bool isSubClass = mono_class_is_subclass_of(script->_class, _class, true);

				if (script->_class == _class || isSubClass)
				{
					outType = script->object;
					break;
				}
			}
		}

		////////////////////

		int j = 0;
		auto children = curNode->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			nstack.insert(nstack.begin() + j, nd);
		}
	}

	nstack.clear();
}

void searchTypesRecursive(SceneNode* root, MonoClass* _class, std::vector<MonoObject*>& outList, bool findInactive)
{
	std::vector<SceneNode*> nstack;

	auto children = root->getChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
		nstack.push_back((SceneNode*)*it);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////

		if (!findInactive)
		{
			if (!curNode->getVisible())
				continue;
		}

		for (auto _it = curNode->components.begin(); _it != curNode->components.end(); ++_it)
		{
			Component* comp = *_it;
			if (!findInactive)
			{
				if (!comp->GetEnabled())
					continue;
			}

			if (comp->GetMonoObject() == nullptr)
				continue;

			MonoClass* cls = mono_object_get_class(comp->GetMonoObject());
			bool isSubClass = mono_class_is_subclass_of(cls, _class, true);

			if (cls == _class || isSubClass)
			{
				outList.push_back(comp->GetMonoObject());
				break;
			}
		}

		for (auto _it = curNode->monoScripts.begin(); _it != curNode->monoScripts.end(); ++_it)
		{
			MonoScript* script = *_it;
			if (!findInactive)
			{
				if (!script->enabled)
					continue;
			}

			bool isSubClass = mono_class_is_subclass_of(script->_class, _class, true);

			if (script->_class == _class || isSubClass)
			{
				outList.push_back(script->object);
				break;
			}
		}

		////////////////////

		int j = 0;
		auto children = curNode->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			nstack.insert(nstack.begin() + j, nd);
		}
	}
}

MonoObject* API_MonoBehaviour::findObjectOfTypeT(MonoReflectionType* type, bool findInactive)
{
	MonoType* _type = mono_reflection_type_get_type(type);
	MonoClass* _class = mono_type_get_class(_type);

	MonoObject* obj = nullptr;
	searchTypeRecursive(GetEngine->GetSceneManager()->getRootSceneNode(), _class, obj, findInactive);

	return obj;
}

MonoArray* API_MonoBehaviour::findObjectsOfTypeT(MonoReflectionType* type, bool findInactive)
{
	MonoType* _type = mono_reflection_type_get_type(type);
	MonoClass* _class = mono_type_get_class(_type);

	std::vector<MonoObject*> objs;
	searchTypesRecursive(GetEngine->GetSceneManager()->getRootSceneNode(), _class, objs, findInactive);

	MonoArray* arr = mono_array_new(GetEngine->GetMonoRuntime()->GetDomain(), _class, objs.size());

	int i = 0;
	for (auto it = objs.begin(); it != objs.end(); ++it, ++i)
	{
		mono_array_setref(arr, i, *it);
	}

	return arr;
}
