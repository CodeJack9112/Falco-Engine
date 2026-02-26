#include "PostEffectListener.h"
#include <Mono\include\mono\metadata\debug-helpers.h>

#include "Engine.h"

void PostEffectListener::notifyMaterialRender(uint32 pass_id, MaterialPtr& mat)
{
	if (postEffectMono != nullptr)
	{
		MonoMethodDesc* methodDesc = nullptr;
		MonoMethod* method = nullptr;

		MonoClass* mclass = GetEngine->GetMonoRuntime()->posteffect_class;

		std::string methodDescStr = std::string("PostEffect:CallOnMaterialRender(int,Material)").c_str();
		methodDesc = mono_method_desc_new(methodDescStr.c_str(), false);

		if (methodDesc)
		{
			method = mono_method_desc_search_in_class(methodDesc, mclass);
			mono_method_desc_free(methodDesc);

			if (method)
			{
				int pass = (int)pass_id;
				Material* _mat = mat.getPointer();

				MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->material_class);
				mono_runtime_object_init(obj);
				mono_field_set_value(obj, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&_mat));

				void* args[2] = { &pass, obj };

				mono_runtime_invoke(method, postEffectMono, args, nullptr);
			}
		}
	}
}
