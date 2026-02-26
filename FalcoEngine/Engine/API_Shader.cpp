#include "API_Shader.h"

#include <OgreHighLevelGpuProgram.h>
#include <OgreHighLevelGpuProgramManager.h>

#include "Engine.h"
#include "StringConverter.h"

using namespace Ogre;

void API_Shader::ctor(MonoObject* this_ptr)
{
}

MonoString* API_Shader::getName(MonoObject* this_ptr)
{
	HighLevelGpuProgram* shader = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&shader));

	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(shader->getName()).c_str());
	return str;
}

MonoObject* API_Shader::find(MonoString* name)
{
	std::string shaderName = (const char*)mono_string_to_utf8((MonoString*)name);
	shaderName = CP_SYS(shaderName);

	if (HighLevelGpuProgramManager::getSingleton().resourceExists(shaderName, "Assets"))
	{
		HighLevelGpuProgramPtr sh = HighLevelGpuProgramManager::getSingleton().getByName(shaderName, "Assets");
		HighLevelGpuProgram* shader = sh.getPointer();

		MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->shader_class);
		mono_runtime_object_init(obj);
		mono_field_set_value(obj, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&shader));

		return obj;
	}

	return nullptr;
}

int API_Shader::getType(MonoObject* this_ptr)
{
	HighLevelGpuProgram* shader;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&shader));

	if (shader->getType() == GpuProgramType::GPT_VERTEX_PROGRAM)
		return 0;
	if (shader->getType() == GpuProgramType::GPT_FRAGMENT_PROGRAM)
		return 1;
}
