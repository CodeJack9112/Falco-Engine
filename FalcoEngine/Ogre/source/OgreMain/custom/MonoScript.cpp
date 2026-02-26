#include "MonoScript.h"
#include "../../../../Mono/include/mono/metadata/debug-helpers.h"
#include "OgreSceneManager.h"

MonoScript::MonoScript(Ogre::SceneNode* node)
{
	sceneNode = node;
}

MonoScript::~MonoScript()
{
	DestroyInstance();
}

void MonoScript::CreateInstance()
{
	if (_class == nullptr)
		return;

	object = mono_object_new(SceneManager::monoDomain, _class);

	mono_runtime_object_init(object);
	gcHandle = mono_gchandle_new(object, true); // Prevent this object to be destroyed by garbage collector

	MonoScript* _this = this;
	mono_field_set_value(object, SceneManager::monoNativePtrField, reinterpret_cast<void*>(&sceneNode));
	mono_field_set_value(object, SceneManager::monoScriptPtrField, reinterpret_cast<void*>(&_this));
}

void MonoScript::DestroyInstance()
{
	MonoMethod* method = mono_class_get_method_from_name(SceneManager::monoBehaviourClass, "Destroy", 0);

	if (method)
	{
		if (object != nullptr)
		{
			MonoObject* except = nullptr;
			mono_runtime_invoke(method, object, nullptr, (MonoObject**)&except);
		}
	}

	if (gcHandle > 0)
	{
		mono_gchandle_free(gcHandle);
		gcHandle = 0;
	}

	startExecuted = false;
}