#include "Component.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

Component::Component(SceneNode * parent, MonoClass * monoClass)
{
	parentSceneNode = parent;

	if (monoClass != nullptr)
	{
		Component * comp = this;

		monoObject = mono_object_new(parent->getCreator()->monoDomain, monoClass);
		mono_runtime_object_init(monoObject);
		mono_field_set_value(monoObject, parent->getCreator()->monoComponentThisPtrField, reinterpret_cast<void*>(&comp));
		mono_field_set_value(monoObject, parent->getCreator()->monoComponentNativePtrField, reinterpret_cast<void*>(&parent));
		gcHandle = mono_gchandle_new(monoObject, true);
	}
}

Component::~Component()
{
	mono_thread_attach(GetParentSceneNode()->getCreator()->monoDomain);

	if (gcHandle > 0)
		mono_gchandle_free(gcHandle);
}

void Component::SetEnabled(bool en)
{
	enabled = en;
	StateChanged(en);
}
