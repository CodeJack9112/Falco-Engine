#include "stdafx.h"
#include "API_Texture.h"
#include "Engine.h"
#include "StringConverter.h"

MonoObject* API_Texture::loadFromFile(MonoObject * path)
{
	std::string _path = (const char*)mono_string_to_utf8((MonoString*)path);
	_path = CP_SYS(_path);

	TexturePtr texture = Ogre::TextureManager::getSingleton().load(_path, "Assets");

	if (texture != nullptr)
	{
		MonoObject* _tex = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->texture2d_class);
		mono_runtime_object_init(_tex);
		MonoClassField * _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");
		MonoClassField * _fileName = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_file_path");

		MonoString* _str = (MonoString*)path;

		Texture * texPtr = texture.getPointer();

		mono_field_set_value(_tex, _texPtr, reinterpret_cast<void*>(&texPtr));
		mono_field_set_value(_tex, _fileName, _str);

		return _tex;
	}

	return nullptr;
}

int API_Texture::getWidth(MonoObject * this_ptr)
{
	MonoClass * _class = mono_object_get_class(this_ptr);
	MonoClassField * _texPtr = mono_class_get_field_from_name(_class, "_texPtr");

	Texture * texPtr = nullptr;
	mono_field_get_value(this_ptr, _texPtr, reinterpret_cast<void*>(&texPtr));

	if (texPtr != nullptr)
	{
		return texPtr->getWidth();
	}

	return 0;
}

int API_Texture::getHeight(MonoObject * this_ptr)
{
	MonoClass * _class = mono_object_get_class(this_ptr);
	MonoClassField * _texPtr = mono_class_get_field_from_name(_class, "_texPtr");

	Texture * texPtr = nullptr;
	mono_field_get_value(this_ptr, _texPtr, reinterpret_cast<void*>(&texPtr));

	if (texPtr != nullptr)
	{
		return texPtr->getHeight();
	}

	return 0;
}
