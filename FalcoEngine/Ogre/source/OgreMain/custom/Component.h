#pragma once

#include "OgrePrerequisites.h"

#include "../../../../Mono/include/mono/metadata/object.h"
#include "../../../../Mono/include/mono/metadata/threads.h"

using namespace Ogre;

namespace Ogre
{
	class _OgreExport Component
	{
	private:
		SceneNode* parentSceneNode;
		bool enabled = true;

		MonoObject* monoObject = nullptr;
		uint32_t gcHandle = 0;

	public:
		bool globalDelete = false;

		Component(SceneNode* parent, MonoClass* monoClass = nullptr);

		virtual ~Component();
		virtual std::string GetComponentTypeName() { return "Component"; }

		MonoObject* GetMonoObject() { return monoObject; }

		SceneNode* GetParentSceneNode() { return parentSceneNode; }
		virtual void SceneLoaded() {}
		virtual void StateChanged(bool active) {}
		virtual void NodeStateChanged(bool active) {}

		void SetEnabled(bool en);
		bool GetEnabled() { return enabled; }
	};
}