#include "API.h"
#include "Engine.h"

void API::APIFuncs::setEnabled(MonoObject* this_ptr, bool enabled)
{
	MonoScript* script = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->script_ptr_field, reinterpret_cast<void*>(&script));

	if (script != nullptr)
	{
		bool _enabled = script->enabled;
		script->enabled = enabled;

		if (enabled == true && !_enabled)
		{
			if (script->sceneNode->getVisible())
			{
				if (!script->startExecuted)
				{
					script->startExecuted = true;
					GetEngine->GetMonoRuntime()->Execute(this_ptr, "Start");
				}
			}
		}
	}
}
