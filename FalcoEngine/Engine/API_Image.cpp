#include "API_Image.h"
#include "UIImage.h"
#include "Engine.h"
#include <Engine\StringConverter.h>
#include <iostream>

MonoObject* API_Image::getImage(MonoObject* this_ptr)
{
	UIImage* image;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&image));

	TexturePtr texture = image->GetImage();

	if (texture != nullptr)
	{
		MonoObject* _tex = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->texture2d_class);
		mono_runtime_object_init(_tex);
		MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");
		MonoClassField* _fileName = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_file_path");

		MonoString* _str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(texture->getName()).c_str());

		Texture* texPtr = texture.getPointer();

		mono_field_set_value(_tex, _texPtr, reinterpret_cast<void*>(&texPtr));
		mono_field_set_value(_tex, _fileName, _str);

		return _tex;
	}

	return nullptr;
}

void API_Image::setImage(MonoObject* this_ptr, MonoObject* texture)
{
	UIImage* image;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&image));

	MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");

	Texture* _tex;
	mono_field_get_value(texture, _texPtr, reinterpret_cast<void*>(&_tex));

	image->SetImage(_tex->getName());
}
